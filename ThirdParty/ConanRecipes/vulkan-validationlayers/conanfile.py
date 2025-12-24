from os import mkdir

from conan import ConanFile
from conan.tools.build import check_min_cppstd, can_run
from conan.tools.cmake import cmake_layout, CMakeToolchain, CMakeDeps, CMake
from conan.tools.files import apply_conandata_patches, export_conandata_patches
from conan.tools.scm import Git
import os

required_conan_version = ">=2.0.9"

class VulkanValidationLayersConan(ConanFile):
    name = "vulkan-validationlayers"
    description = "vulkan validation layers"
    license = "https://github.com/KhronosGroup/Vulkan-ValidationLayers?tab=License-1-ov-file"
    url = "https://github.com/conan-io/conan-center-index"
    homepage = "https://github.com/KhronosGroup/Vulkan-ValidationLayers"
    topics = ("vulkan", "debug")
    package_type = "shared-library"
    settings = "os", "arch", "compiler", "build_type"
    options = {}
    default_options = {}

    def export_sources(self):
        export_conandata_patches(self)

    def layout(self):
        cmake_layout(self, src_folder="src")

    def validate(self):
        check_min_cppstd(self, 17)

    def requirements(self):
        base_version = self.version.replace("-exp", "")
        self.requires(f"vulkan-headers/{base_version}")
        self.requires(f"spirv-headers/{base_version}")
        self.requires(f"spirv-tools/{base_version}")
        self.requires(f"vulkan-utility-libraries/{self.version}")

    def build_requirements(self):
        self.tool_requires("ninja/[>=1.12]")
        self.tool_requires("cmake/[>=3.16]")

    def source(self):
        git = Git(self)
        git.clone("https://github.com/KhronosGroup/Vulkan-ValidationLayers", target=".", args=["-b", self.conan_data["sources"][self.version]["branch"]])
        apply_conandata_patches(self)

    def generate(self):
        cmake_toolchain = CMakeToolchain(self, generator="Ninja")
        cmake_toolchain.cache_variables["UPDATE_DEPS"] = "OFF"
        cmake_toolchain.cache_variables["BUILD_WERROR"] = "OFF"
        cmake_toolchain.cache_variables["BUILD_TESTS"] = "OFF"
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
        os.mkdir(os.path.join(self.package_folder, "include"))

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "VulkanValidationLayers")
        self.cpp_info.bindirs = ["bin"]
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.libs = []
        self.cpp_info.libdirs = []

    def test(self):
        if can_run(self):
            bin_path = os.path.join(self.cpp.build.bindirs[0], "test_package")
            self.run(bin_path, env="conanrun")
