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

Before building each recipe the script exports it, resolves the package id for the
current profile, then asks both the local cache and the remote whether that exact
recipe-revision + package-id binary already exists. A recipe found on the remote is
skipped with no download, rebuild, or no-op re-upload; one found only in the local
cache skips the rebuild but is still uploaded so the local binary reaches the remote.
Pass --no-skip-existing to force every supported recipe through 'conan create'.
"""

from __future__ import annotations

import argparse
import json
import platform
import re
import subprocess
import sys
import time
from pathlib import Path

import yaml


SUPPORTED_PLATFORMS = ("Windows-x86_64", "Macos-armv8", "Linux-x86_64")


def current_platform() -> str:
    system = platform.system()
    machine = platform.machine().lower()
    if system == "Windows" and machine in ("amd64", "x86_64"):
        return "Windows-x86_64"
    if system == "Darwin" and machine in ("arm64", "aarch64"):
        return "Macos-armv8"
    if system == "Linux" and machine in ("amd64", "x86_64"):
        return "Linux-x86_64"
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


def run_capture(cmd: list[str], cwd: Path | None = None) -> tuple[int, str]:
    print(f"\n$ {' '.join(cmd)}", flush=True)
    proc = subprocess.run(cmd, cwd=str(cwd) if cwd else None, stdout=subprocess.PIPE, text=True)
    return proc.returncode, proc.stdout


def graph_recipe_node(graph_json: str, reference: str) -> dict | None:
    try:
        nodes = json.loads(graph_json).get("graph", {}).get("nodes", {})
    except (json.JSONDecodeError, AttributeError):
        return None
    for node in nodes.values():
        if (node.get("ref") or "").split("#", 1)[0] == reference:
            return node
    return None


def list_has_binary(list_json: str, reference: str, package_id: str) -> bool:
    # 'conan list' reports each scope (the local cache or a remote) as either an {"error": ...} payload
    # when the reference is absent, or a revisions -> packages tree when it is present; the binary exists
    # there only if our package id shows up under some revision of that tree.
    name_version = reference.split("#", 1)[0]
    try:
        data = json.loads(list_json)
    except json.JSONDecodeError:
        return False
    for scope in data.values():
        if not isinstance(scope, dict) or "error" in scope:
            continue
        revisions = (scope.get(name_version) or {}).get("revisions") or {}
        for revision in revisions.values():
            if package_id in (revision.get("packages") or {}):
                return True
    return False


def conan_list_has(args: argparse.Namespace, reference: str, package_id: str, remote: str | None) -> bool:
    cmd = [args.conan, "list", f"{reference}:{package_id}", "--format=json"]
    if remote:
        cmd += ["-r", remote]
    code, out = run_capture(cmd, cwd=args.recipes_root)
    if code != 0:
        return False
    return list_has_binary(out, reference, package_id)


def package_status(args: argparse.Namespace, recipe: Recipe, create_extra: list[str]) -> str:
    # Export so the local recipe revision (a hash of the recipe's contents) lands in the cache, then
    # resolve the package id for the current profile via 'conan graph info'. With both in hand, query the
    # local cache and (when a remote is configured) the remote for that exact recipe-revision +
    # package-id binary -- 'conan graph info' alone cannot tell the two apart, since a cache hit masks the
    # remote. Returns "remote" if published, "local" if only cached locally, else "build". Any
    # uncertainty -- a failed export/query, unparseable output, a missing id -- yields "build", so the
    # pre-check only ever short-circuits a sure hit, never a guess.
    export = [args.conan, "export", f"{recipe.dir_name}/conanfile.py", "--version", recipe.version]
    if run(export, cwd=args.recipes_root) != 0:
        return "build"

    info = [args.conan, "graph", "info", f"--requires={recipe.reference}", "--format=json"]
    if args.remote:
        info += ["-r", args.remote]
    info += create_extra
    code, out = run_capture(info, cwd=args.recipes_root)
    if code != 0:
        return "build"

    node = graph_recipe_node(out, recipe.reference)
    reference = (node or {}).get("ref") or ""
    package_id = (node or {}).get("package_id")
    if "#" not in reference or not package_id:
        return "build"

    if args.remote and conan_list_has(args, reference, package_id, args.remote):
        return "remote"
    if conan_list_has(args, reference, package_id, None):
        return "local"
    return "build"


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
        "--profile",
        action="append",
        default=[],
        help="conan profile applied to both host and build (repeatable)",
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
    parser.add_argument(
        "--skip-existing",
        action=argparse.BooleanOptionalAction,
        default=True,
        help="before building, query the remote and skip any recipe whose binary is already published",
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
    local: list[Recipe] = []
    present: list[Recipe] = []

    create_extra: list[str] = []
    for profile in args.profile:
        create_extra += ["-pr:h", profile, "-pr:b", profile]
    create_extra += args.conan_args

    for index, recipe in enumerate(recipes, start=1):
        header = f"[{index}/{len(recipes)}] {recipe.name}"
        if recipe.version is None:
            fail(args, built, skipped, local, present, recipe,
                 f"could not determine latest version from {recipe.conandata}")
        if not recipe.supports(host):
            reason = f"not built on {host} (platforms: {recipe.platforms or 'none'})"
            print(f"\n=== {header} -- SKIP: {reason} ===", flush=True)
            skipped.append((recipe, reason))
            continue

        if args.skip_existing:
            status = package_status(args, recipe, create_extra)
            if status == "remote":
                print(f"\n=== {header} -- {recipe.reference} already on remote, skipping ===", flush=True)
                present.append(recipe)
                continue
            if status == "local":
                print(f"\n=== {header} -- {recipe.reference} only in local cache, skipping build ===",
                      flush=True)
                local.append(recipe)
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
            fail(args, built, skipped, local, present, recipe,
                 f"'conan create' exited with code {code} after {elapsed:.0f}s")
        print(f"--- {recipe.reference} built in {elapsed:.0f}s ---", flush=True)
        built.append(recipe)

    return built, skipped, local, present


def fail(args, built, skipped, local, present, recipe: Recipe, message: str):
    print(f"\n!!! BUILD FAILED: {recipe.name} -- {message}", file=sys.stderr, flush=True)
    print_summary(built, skipped, local, present, failed=recipe)
    if args.upload:
        print("\nUpload skipped: not all recipes built successfully.", flush=True)
    sys.exit(1)


def configure_remote(args: argparse.Namespace):
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


def upload_all(args: argparse.Namespace, recipes: list[Recipe]):
    print("\n=== uploading packages ===", flush=True)
    for recipe in recipes:
        print(f"\n--- uploading {recipe.reference} ---", flush=True)
        if run([args.conan, "upload", recipe.reference, "-r", args.remote, "--confirm"]) != 0:
            sys.exit(f"error: failed to upload {recipe.reference}")
    print(f"\nUploaded {len(recipes)} package(s) to '{args.remote}'.", flush=True)


def print_summary(built, skipped, local, present, failed: Recipe | None = None):
    print("\n" + "=" * 60, flush=True)
    print("SUMMARY", flush=True)
    print("=" * 60, flush=True)
    for recipe in built:
        print(f"  [OK]      {recipe.reference}", flush=True)
    for recipe in present:
        print(f"  [REMOTE]  {recipe.reference} (already published)", flush=True)
    for recipe in local:
        print(f"  [LOCAL]   {recipe.reference} (only in local cache)", flush=True)
    for recipe, reason in skipped:
        print(f"  [SKIP]    {recipe.name} ({reason})", flush=True)
    if failed is not None:
        print(f"  [FAILED]  {failed.reference}", flush=True)
    print(
        f"\nbuilt: {len(built)}  on-remote: {len(present)}  local-only: {len(local)}  "
        f"skipped: {len(skipped)}" + ("  failed: 1" if failed is not None else ""),
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

    # Log in before building so the per-recipe pre-check can query the remote for existing binaries.
    if args.upload:
        configure_remote(args)

    built, skipped, local, present = build_all(args, recipes, host)
    print_summary(built, skipped, local, present)

    if args.upload:
        to_upload = built + local
        if not to_upload:
            print("\nNothing to upload; every recipe was already on the remote.", flush=True)
        else:
            upload_all(args, to_upload)
    print("\nAll done.", flush=True)


if __name__ == "__main__":
    main()
