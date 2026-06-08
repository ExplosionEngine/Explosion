from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.files import copy, get
import os

required_conan_version = ">=2.0.9"


class LibclangConan(ConanFile):
    name = "libclang"
    description = "LLVM libclang (prebuilt, repackaged from llvm-project release artifacts)"
    license = "Apache-2.0 WITH LLVM-exception"
    url = "https://github.com/llvm/llvm-project"
    homepage = "https://github.com/llvm/llvm-project"
    topics = ("llvm", "clang", "tool", "prebuilt")
    package_type = "shared-library"
    settings = "os", "arch", "compiler", "build_type"
    no_copy_source = True

    def _source_entry(self):
        sources = self.conan_data.get("sources", {}).get(str(self.version), {})
        return sources.get(f"{self.settings.os}-{self.settings.arch}")

    def validate(self):
        if self._source_entry() is None:
            available = sorted(
                self.conan_data.get("sources", {}).get(str(self.version), {})
            )
            raise ConanInvalidConfiguration(
                f"libclang/{self.version} prebuilt is not available for "
                f"{self.settings.os}/{self.settings.arch}. Supported: {available}"
            )

    def build(self):
        get(self, **self._source_entry(), strip_root=True, destination=self.build_folder)

    def package(self):
        src = self.build_folder
        # Public C API headers; skip the multi-GB clang/llvm internal headers.
        copy(self, "clang-c/*",
             os.path.join(src, "include"),
             os.path.join(self.package_folder, "include"))
        if self.settings.os == "Windows":
            copy(self, "libclang.dll", os.path.join(src, "bin"),
                 os.path.join(self.package_folder, "bin"))
            copy(self, "libclang.lib", os.path.join(src, "lib"),
                 os.path.join(self.package_folder, "lib"))
        elif self.settings.os == "Linux":
            copy(self, "libclang.so*", os.path.join(src, "lib"),
                 os.path.join(self.package_folder, "lib"))
            self._ensure_unversioned_symlink("libclang.so")
        elif self.settings.os == "Macos":
            copy(self, "libclang*.dylib", os.path.join(src, "lib"),
                 os.path.join(self.package_folder, "lib"))
            self._ensure_unversioned_symlink("libclang.dylib")
        # License: Linux/macOS CPack tarballs put it at the root, the Windows
        # tarball nests it under include/llvm/Support/.
        for rel in ("LICENSE.TXT", "include/llvm/Support/LICENSE.TXT"):
            full = os.path.join(src, rel)
            if os.path.isfile(full):
                copy(self, os.path.basename(rel), os.path.dirname(full),
                     os.path.join(self.package_folder, "licenses"))
                break

    def package_id(self):
        # libclang is C ABI; the same prebuilt serves any compiler/build_type.
        del self.info.settings.compiler
        del self.info.settings.build_type

    def package_info(self):
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.libdirs = ["lib"]
        if self.settings.os == "Windows":
            self.cpp_info.libs = ["libclang"]
            self.cpp_info.bindirs = ["bin"]
        else:
            self.cpp_info.libs = ["clang"]
            self.cpp_info.bindirs = ["lib"]

    def _ensure_unversioned_symlink(self, base):
        """LLVM CPack tarballs ship libclang.so.X / libclang.X.dylib but may
        omit the unversioned symlink CMakeDeps needs to resolve `-lclang`."""
        pkg_lib = os.path.join(self.package_folder, "lib")
        target = os.path.join(pkg_lib, base)
        if not os.path.isdir(pkg_lib) or os.path.lexists(target):
            return
        candidates = sorted(f for f in os.listdir(pkg_lib)
                            if f.startswith(base) and f != base)
        if candidates:
            os.symlink(candidates[-1], target)
