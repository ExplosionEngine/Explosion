from conan import ConanFile
from conan.tools.build import check_min_cppstd
from conan.tools.build import can_run
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import apply_conandata_patches, get, collect_libs
import os

required_conan_version = ">=2.0.9"

class AssimpConan(ConanFile):
    name = "assimp"
    description = "assimp"
    license = "https://github.com/assimp/assimp?tab=License-1-ov-file"
    url = "https://github.com/conan-io/conan-center-index"
    homepage = "https://github.com/assimp/assimp"
    topics = ("model", "loader", "tool")
    package_type = "shared-library"
    settings = "os", "arch", "compiler", "build_type"

    def layout(self):
        cmake_layout(self, src_folder="src")

    def validate(self):
        check_min_cppstd(self, 17)

    def requirements(self):
        self.requires("zlib/1.3.1")

    def build_requirements(self):
        self.tool_requires("ninja/[>=1.12]")
        self.tool_requires("cmake/[>=3.16]")

    def source(self):
        get(self, **self.conan_data["sources"][self.version], strip_root=True)
        apply_conandata_patches(self)

    def generate(self):
        cmake_toolchain = CMakeToolchain(self, generator="Ninja")
        cmake_toolchain.cache_variables["ASSIMP_BUILD_TESTS"] = "OFF"
        cmake_toolchain.cache_variables["ASSIMP_WARNINGS_AS_ERRORS"] = "OFF"
        cmake_toolchain.cache_variables["ASSIMP_INSTALL_PDB"] = "OFF"
        cmake_toolchain.cache_variables["ASSIMP_BUILD_ZLIB"] = "OFF"
        cmake_toolchain.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = collect_libs(self)
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.libdirs = ["lib"]
        if self.settings.os == "Windows":
            self.cpp_info.bindirs = ["bin"]

    def test(self):
        if can_run(self):
            bin_path = os.path.join(self.cpp.build.bindirs[0], "test_package")
            self.run(bin_path, env="conanrun")
