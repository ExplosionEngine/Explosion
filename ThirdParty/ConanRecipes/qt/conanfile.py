from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.build import check_min_cppstd
from conan.tools.files import copy
import os
import sys
import subprocess

required_conan_version = ">=2.0.9"


class QtConan(ConanFile):
    name = "qt"
    description = "Qt 6 (prebuilt, repackaged from the official Qt CDN via aqtinstall)"
    license = "https://github.com/qt/qt5/tree/dev/LICENSES"
    url = "https://www.qt.io/"
    homepage = "https://www.qt.io/"
    topics = ("gui", "tool", "prebuilt")
    package_type = "shared-library"
    settings = "os", "arch", "compiler", "build_type"
    no_copy_source = True

    # Modules added on top of aqt's default base install. The base set already
    # includes qtbase, qtdeclarative, qttools, qtsvg, qttranslations, qtimageformats.
    _ADDON_MODULES = (
        "qtshadertools",
        "qtpositioning",
        "qtwebchannel",
        "qtwebengine",
        "qtwebsockets",
    )

    # (os, arch) -> (aqt host, aqt target, aqt arch, install-subdir-name).
    # Supported: Windows x64, Linux x64, macOS arm64.
    _AQT_PLATFORM = {
        ("Windows", "x86_64"): ("windows", "desktop", "win64_msvc2022_64", "msvc2022_64"),
        ("Linux",   "x86_64"): ("linux",   "desktop", "linux_gcc_64",      "gcc_64"),
        ("Macos",   "armv8"):  ("mac",     "desktop", "clang_64",          "macos"),
    }

    def _aqt_params(self):
        return self._AQT_PLATFORM.get((str(self.settings.os), str(self.settings.arch)))

    @property
    def _qt_version(self):
        """Strip the -exp / +foo suffix so aqt sees a plain version like 6.10.1."""
        v = str(self.version)
        for sep in ("-", "+"):
            v = v.split(sep, 1)[0]
        return v

    def validate(self):
        check_min_cppstd(self, 17)
        if self._aqt_params() is None:
            raise ConanInvalidConfiguration(
                f"qt/{self.version} prebuilt is not available for "
                f"{self.settings.os}/{self.settings.arch}. "
                f"Supported: {sorted(self._AQT_PLATFORM)}"
            )
        if self.settings.os == "Windows" and str(self.settings.compiler) == "msvc":
            # The win64_msvc2022_64 prebuilt links/runs against any MSVC toolset
            # that is binary-compatible with v143. Microsoft's C++ binary
            # compatibility guarantee covers v140-v145 (see "C++ binary
            # compatibility 2015-2026" on Microsoft Learn), so VS2022 (193/194)
            # and VS2026 (195+) both consume this prebuilt safely as long as Qt
            # keeps shipping only win64_msvc2022_64. If Qt later adds a separate
            # win64_msvc2026_64 in aqtinstall, switch _AQT_PLATFORM accordingly.
            ver = int(str(self.settings.compiler.version))
            if ver < 193:
                raise ConanInvalidConfiguration(
                    f"Qt prebuilt is built with MSVC 2022 (v143). "
                    f"compiler.version must be >= 193 (VS2022) for ABI "
                    f"compatibility, got {ver}"
                )

    def build(self):
        self._ensure_aqt()
        host, target, arch, dirname = self._aqt_params()
        outdir = os.path.join(self.build_folder, "qt-install")
        cmd = [
            sys.executable, "-m", "aqt", "install-qt",
            host, target, self._qt_version, arch,
            "-O", outdir,
            "-m", *self._ADDON_MODULES,
        ]
        self.output.info(f"Running aqt: {' '.join(cmd)}")
        subprocess.check_call(cmd)
        self._strip_debug_artifacts(os.path.join(outdir, self._qt_version, dirname))

    def package(self):
        _, _, _, dirname = self._aqt_params()
        qt_prefix = os.path.join(self.build_folder, "qt-install", self._qt_version, dirname)
        if not os.path.isdir(qt_prefix):
            raise RuntimeError(f"aqt did not produce the expected layout at {qt_prefix}")
        copy(self, "*", qt_prefix, self.package_folder, keep_path=True)
        # aqt 3.3+ already writes a relocatable bin/qt.conf; idempotent rewrite
        # so we don't depend on that behavior surviving aqt upgrades.
        qt_conf = os.path.join(self.package_folder, "bin", "qt.conf")
        os.makedirs(os.path.dirname(qt_conf), exist_ok=True)
        with open(qt_conf, "w", encoding="utf-8", newline="\n") as f:
            f.write("[Paths]\nPrefix=..\n")

    def package_id(self):
        # All consumer build_types map to Release in the top-level project, and
        # the prebuilt is fully baked, so compiler/build_type don't affect the
        # binary. validate() filters incompatible toolchains.
        del self.info.settings.build_type
        del self.info.settings.compiler

    def package_info(self):
        # Consumers must use Qt's own Qt6Config.cmake (which is relocatable).
        # We expose lib/cmake via builddirs so CMAKE_PREFIX_PATH picks it up.
        self.cpp_info.libs = []
        self.cpp_info.builddirs = [os.path.join("lib", "cmake")]
        self.cpp_info.set_property("cmake_find_mode", "none")

    def _ensure_aqt(self):
        """pip-install aqtinstall on demand."""
        try:
            import aqt  # noqa: F401
            return
        except ImportError:
            pass
        cmd = [sys.executable, "-m", "pip", "install", "aqtinstall"]
        if self.settings.os != "Windows":
            cmd.append("--break-system-packages")
        subprocess.check_call(cmd)

    def _strip_debug_artifacts(self, root_dir):
        """Drop Qt's Debug variants (~2 GiB on Windows). The top-level project
        pins all Conan consumption to Release (CONAN_INSTALL_BUILD_CONFIGURATIONS
        + CMAKE_MAP_IMPORTED_CONFIG_*), so debug DLLs/libs/.prl plus per-config
        Targets-debug.cmake imports are dead weight.

        Patterns:
          1. ``<stem>d.<ext>`` when ``<stem>.<ext>`` exists in the same dir
             (Qt's Windows debug suffix; pairs like Qt6Core.dll/Qt6Cored.dll).
             The "release counterpart exists" check protects names that
             genuinely end in ``d`` -- e.g. qdirect2d.dll, qopensslbackend.dll.
          2. ``<stem>Targets-debug.cmake`` -- per-config CMake target imports.

        Idempotent.
        """
        debug_exts = {".dll", ".lib", ".pdb", ".prl", ".so", ".dylib", ".a"}
        deleted, saved = 0, 0
        for cur, _, files in os.walk(root_dir):
            names = set(files)
            for fname in files:
                stem, ext = os.path.splitext(fname)
                ext_l = ext.lower()
                is_debug_cmake = ext_l == ".cmake" and stem.endswith("-debug")
                is_debug_binary = (
                    ext_l in debug_exts
                    and stem.endswith("d") and len(stem) > 1
                    and (stem[:-1] + ext) in names
                )
                if not (is_debug_cmake or is_debug_binary):
                    continue
                full = os.path.join(cur, fname)
                saved += os.path.getsize(full)
                os.remove(full)
                deleted += 1
        if deleted:
            self.output.info(
                f"Stripped {deleted} debug artifacts ({saved / 1024 / 1024:.1f} MiB)"
            )
