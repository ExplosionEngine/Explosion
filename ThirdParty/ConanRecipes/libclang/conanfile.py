from conan import ConanFile
from conan.tools.build import check_min_cppstd
from conan.tools.build import can_run
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import apply_conandata_patches, get, copy
import os

required_conan_version = ">=2.0.9"

class LibclangConan(ConanFile):
    name = "libclang"
    description = "llvm - libclang"
    license = "https://github.com/llvm/llvm-project/blob/main/LICENSE.TXT"
    url = "https://github.com/conan-io/conan-center-index"
    homepage = "https://github.com/llvm/llvm-project"
    topics = ("llvm", "tool")
    package_type = "shared-library"
    settings = "os", "arch", "compiler", "build_type"

    def layout(self):
        cmake_layout(self, src_folder="src")

    def validate(self):
        check_min_cppstd(self, 17)

    def build_requirements(self):
        self.tool_requires("ninja/[>=1.12]")
        self.tool_requires("cmake/[>=3.16]")

    def source(self):
        get(self, **self.conan_data["sources"][self.version], strip_root=True)
        apply_conandata_patches(self)

    def generate(self):
        cmake_toolchain = CMakeToolchain(self, generator="Ninja")
        cmake_toolchain.cache_variables["LLVM_ENABLE_PROJECTS"] = "clang"
        cmake_toolchain.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        install_folder = os.path.join(self.build_folder, "installed")
        cmake.configure(build_script_folder=os.path.join(self.source_folder, "llvm"), cli_args=[f"-DCMAKE_INSTALL_PREFIX={install_folder}"])
        cmake.build(target="libclang")
        cmake.build(target="install-libclang")
        cmake.build(target="install-libclang-headers")

    def package(self):
        copy(self, "*", os.path.join(self.build_folder, "installed"), self.package_folder)

    def package_info(self):
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.libdirs = ["lib"]
        if self.settings.os == "Windows":
            self.cpp_info.libs = ["libclang"]
            self.cpp_info.bindirs = ["bin"]
        else:
            self.cpp_info.libs = ["clang"]

    def test(self):
        if can_run(self):
            bin_path = os.path.join(self.cpp.build.bindirs[0], "test_package")
            self.run(bin_path, env="conanrun")
