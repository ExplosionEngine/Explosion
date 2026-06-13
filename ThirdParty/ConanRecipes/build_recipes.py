#!/usr/bin/env python3
"""Build (and optionally upload) every Conan recipe under ThirdParty/ConanRecipes.

Each recipe is built one-by-one in dependency order for the latest version (the
top entry in conandata.yml), filtered by the recipe's `platforms` list. The
first failure stops the run and prints a summary. Uploading is opt-in and only
runs once every recipe has built successfully.

With --export-only the script just runs 'conan export' for every version of
every recipe, so a subsequent 'conan install --build=missing' resolves recipes
from the local cache and builds whatever the remote has no binaries for. CI
uses this to validate recipe changes together with the engine build.
"""

from __future__ import annotations

import argparse
import platform
import re
import subprocess
import sys
import time
from pathlib import Path

import yaml


SUPPORTED_PLATFORMS = ("Windows-x86_64", "Macos-armv8")


def current_platform() -> str:
    system = platform.system()
    machine = platform.machine().lower()
    if system == "Windows" and machine in ("amd64", "x86_64"):
        return "Windows-x86_64"
    if system == "Darwin" and machine in ("arm64", "aarch64"):
        return "Macos-armv8"
    sys.exit(
        f"error: unsupported host {system}/{platform.machine()}; "
        f"only {', '.join(SUPPORTED_PLATFORMS)} are supported"
    )


class Recipe:
    def __init__(self, path: Path):
        self.path = path
        self.dir_name = path.name
        self.conanfile = path / "conanfile.py"
        self.conandata = path / "conandata.yml"

        data = yaml.safe_load(self.conandata.read_text(encoding="utf-8")) or {}
        self.name = self._parse_name() or self.dir_name
        self.versions = list_versions(data)
        self.version = self.versions[0] if self.versions else None
        self.platforms = data.get("platforms") or []
        self.requires = data.get("requires") or []

    def _parse_name(self) -> str | None:
        text = self.conanfile.read_text(encoding="utf-8")
        match = re.search(r"""^\s*name\s*=\s*["']([^"']+)["']""", text, re.MULTILINE)
        return match.group(1) if match else None

    def supports(self, host: str) -> bool:
        return host in self.platforms

    @property
    def reference(self) -> str:
        return f"{self.name}/{self.version}"


def list_versions(data: dict) -> list[str]:
    sources = data.get("sources")
    if isinstance(sources, dict) and sources:
        return [str(v) for v in sources]
    versions = data.get("versions")
    if isinstance(versions, list) and versions:
        return [str(v) for v in versions]
    return []


def discover_recipes(root: Path) -> list[Recipe]:
    return [
        Recipe(child)
        for child in sorted(p for p in root.iterdir() if p.is_dir())
        if (child / "conanfile.py").is_file() and (child / "conandata.yml").is_file()
    ]


def order_by_dependencies(recipes: list[Recipe]) -> list[Recipe]:
    by_name = {r.name: r for r in recipes}
    ordered: list[Recipe] = []
    visited: set[str] = set()
    visiting: set[str] = set()

    def visit(recipe: Recipe):
        if recipe.name in visited:
            return
        if recipe.name in visiting:
            raise SystemExit(f"error: dependency cycle involving '{recipe.name}'")
        visiting.add(recipe.name)
        for dep in sorted(recipe.requires):
            if dep in by_name:
                visit(by_name[dep])
        visiting.discard(recipe.name)
        visited.add(recipe.name)
        ordered.append(recipe)

    for recipe in recipes:
        visit(recipe)
    return ordered


def run(cmd: list[str], cwd: Path | None = None, redact: set[str] | None = None) -> int:
    shown = " ".join("***" if redact and arg in redact else arg for arg in cmd)
    print(f"\n$ {shown}", flush=True)
    return subprocess.run(cmd, cwd=str(cwd) if cwd else None).returncode


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Build and optionally upload all Conan recipes.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        "--recipes-root",
        type=Path,
        default=Path(__file__).resolve().parent,
        help="directory containing the recipe sub-directories",
    )
    parser.add_argument("--conan", default="conan", help="path to the conan executable")
    parser.add_argument(
        "--build", default="missing", help="conan --build policy (passed as --build=<value>)"
    )
    parser.add_argument(
        "--profile", action="append", default=[], help="conan profile (repeatable)"
    )
    parser.add_argument(
        "--conan-arg",
        action="append",
        default=[],
        dest="conan_args",
        help="extra raw argument forwarded to 'conan create' (repeatable)",
    )
    parser.add_argument(
        "--only", action="append", default=[], help="only build these recipe names (repeatable)"
    )
    parser.add_argument(
        "--skip", action="append", default=[], help="skip these recipe names (repeatable)"
    )
    parser.add_argument(
        "--export-only",
        action="store_true",
        help="only 'conan export' every version of every recipe; no build, no platform filter",
    )

    upload = parser.add_argument_group("upload")
    upload.add_argument(
        "--upload",
        action="store_true",
        help="upload all packages after every recipe built successfully",
    )
    upload.add_argument("--remote", help="conan remote name to upload to")
    upload.add_argument(
        "--remote-url", help="if given, register/update the remote with this URL before login"
    )
    upload.add_argument("--remote-user", help="username for the remote")
    upload.add_argument("--remote-password", help="password for the remote")

    args = parser.parse_args()
    if args.upload and not args.remote:
        parser.error("--upload requires --remote")
    if args.export_only and args.upload:
        parser.error("--export-only cannot be combined with --upload")
    return args


def export_all(args: argparse.Namespace, recipes: list[Recipe]):
    count = 0
    for recipe in recipes:
        if not recipe.versions:
            sys.exit(f"error: could not determine versions from {recipe.conandata}")
        for version in recipe.versions:
            cmd = [args.conan, "export", f"{recipe.dir_name}/conanfile.py", "--version", version]
            if run(cmd, cwd=args.recipes_root) != 0:
                sys.exit(f"error: failed to export {recipe.name}/{version}")
            count += 1
    print(f"\nExported {count} recipe version(s).", flush=True)


def build_all(args: argparse.Namespace, recipes: list[Recipe], host: str):
    built: list[Recipe] = []
    skipped: list[tuple[Recipe, str]] = []

    create_extra: list[str] = []
    for profile in args.profile:
        create_extra += ["-pr", profile]
    create_extra += args.conan_args

    for index, recipe in enumerate(recipes, start=1):
        header = f"[{index}/{len(recipes)}] {recipe.name}"
        if recipe.version is None:
            fail(args, built, skipped, recipe,
                 f"could not determine latest version from {recipe.conandata}")
        if not recipe.supports(host):
            reason = f"not built on {host} (platforms: {recipe.platforms or 'none'})"
            print(f"\n=== {header} -- SKIP: {reason} ===", flush=True)
            skipped.append((recipe, reason))
            continue

        print(f"\n=== {header} -- building {recipe.reference} ===", flush=True)
        start = time.time()
        cmd = [
            args.conan, "create", f"{recipe.dir_name}/conanfile.py",
            "--version", recipe.version,
            f"--build={args.build}",
        ] + create_extra
        code = run(cmd, cwd=args.recipes_root)
        elapsed = time.time() - start
        if code != 0:
            fail(args, built, skipped, recipe,
                 f"'conan create' exited with code {code} after {elapsed:.0f}s")
        print(f"--- {recipe.reference} built in {elapsed:.0f}s ---", flush=True)
        built.append(recipe)

    return built, skipped


def fail(args, built, skipped, recipe: Recipe, message: str):
    print(f"\n!!! BUILD FAILED: {recipe.name} -- {message}", file=sys.stderr, flush=True)
    print_summary(built, skipped, failed=recipe)
    if args.upload:
        print("\nUpload skipped: not all recipes built successfully.", flush=True)
    sys.exit(1)


def upload_all(args: argparse.Namespace, built: list[Recipe]):
    print("\n=== uploading packages ===", flush=True)

    if args.remote_url:
        if run([args.conan, "remote", "add", "--force", args.remote, args.remote_url]) != 0:
            sys.exit("error: failed to register remote")

    if args.remote_user is not None:
        login = [args.conan, "remote", "login", args.remote, args.remote_user]
        redact: set[str] = set()
        if args.remote_password is not None:
            login += ["-p", args.remote_password]
            redact.add(args.remote_password)
        if run(login, redact=redact) != 0:
            sys.exit("error: failed to log in to remote")

    for recipe in built:
        print(f"\n--- uploading {recipe.reference} ---", flush=True)
        if run([args.conan, "upload", recipe.reference, "-r", args.remote, "--confirm"]) != 0:
            sys.exit(f"error: failed to upload {recipe.reference}")
    print(f"\nUploaded {len(built)} package(s) to '{args.remote}'.", flush=True)


def print_summary(built, skipped, failed: Recipe | None = None):
    print("\n" + "=" * 60, flush=True)
    print("SUMMARY", flush=True)
    print("=" * 60, flush=True)
    for recipe in built:
        print(f"  [OK]      {recipe.reference}", flush=True)
    for recipe, reason in skipped:
        print(f"  [SKIP]    {recipe.name} ({reason})", flush=True)
    if failed is not None:
        print(f"  [FAILED]  {failed.reference}", flush=True)
    print(
        f"\nbuilt: {len(built)}  skipped: {len(skipped)}"
        + ("  failed: 1" if failed is not None else ""),
        flush=True,
    )


def main():
    args = parse_args()
    root: Path = args.recipes_root
    if not root.is_dir():
        sys.exit(f"error: recipes root not found: {root}")

    recipes = discover_recipes(root)
    if args.only:
        recipes = [r for r in recipes if r.name in args.only or r.dir_name in args.only]
    if args.skip:
        recipes = [r for r in recipes if r.name not in args.skip and r.dir_name not in args.skip]
    if not recipes:
        sys.exit("error: no recipes to build")

    if args.export_only:
        export_all(args, recipes)
        return

    host = current_platform()
    print(f"Host platform: {host}", flush=True)

    recipes = order_by_dependencies(recipes)
    print("Build order: " + ", ".join(r.name for r in recipes), flush=True)

    built, skipped = build_all(args, recipes, host)
    print_summary(built, skipped)

    if args.upload:
        if not built:
            print("\nNothing was built; skipping upload.", flush=True)
        else:
            upload_all(args, built)
    print("\nAll done.", flush=True)


if __name__ == "__main__":
    main()
