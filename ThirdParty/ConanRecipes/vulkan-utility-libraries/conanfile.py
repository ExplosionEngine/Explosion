from conan import ConanFile
from conan.tools.build import check_min_cppstd, can_run
from conan.tools.cmake import cmake_layout, CMakeToolchain, CMakeDeps, CMake
from conan.tools.files import apply_conandata_patches
from conan.tools.scm import Git
import os

required_conan_version = ">=2.0.9"

class VulkanUtilityLibrariesConan(ConanFile):
    name = "vulkan-utility-libraries"
    description = "vulkan utility libraries"
    license = "https://github.com/KhronosGroup/Vulkan-Utility-Libraries?tab=License-1-ov-file"
    url = "https://github.com/conan-io/conan-center-index"
    homepage = "https://github.com/KhronosGroup/Vulkan-Utility-Libraries"
    topics = ("vulkan", "utils")
    package_type = "static-library"
    settings = "os", "arch", "compiler", "build_type"
    options = {}
    default_options = {}

    def layout(self):
        cmake_layout(self, src_folder="src")

    def validate(self):
        check_min_cppstd(self, 17)

    def requirements(self):
        base_version = self.version.replace("-exp", "")
        self.requires(f"vulkan-headers/{base_version}", transitive_headers=True)

    def build_requirements(self):
        self.tool_requires("ninja/[>=1.12]")
        self.tool_requires("cmake/[>=3.16]")

    def source(self):
        git = Git(self)
        git.clone("https://github.com/KhronosGroup/Vulkan-Utility-Libraries", target=".", args=["-b", self.conan_data["sources"][self.version]["branch"]])
        apply_conandata_patches(self)

    def generate(self):
        cmake_toolchain = CMakeToolchain(self, generator="Ninja")
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
        self.cpp_info.set_property("cmake_file_name", "VulkanUtilityLibraries")

        self.cpp_info.components["vulkanlayersettings"].set_property("cmake_target_name", "Vulkan::LayerSettings")
        self.cpp_info.components["vulkanlayersettings"].bindirs = []
        self.cpp_info.components["vulkanlayersettings"].libdirs = ["lib"]
        self.cpp_info.components["vulkanlayersettings"].includedirs = ["include"]
        self.cpp_info.components["vulkanlayersettings"].requires = ["vulkan-headers::vulkanheaders"]
        self.cpp_info.components["vulkanlayersettings"].libs = ["VulkanLayerSettings"]

        self.cpp_info.components["vulkanutilityheaders"].set_property("cmake_target_name", "Vulkan::UtilityHeaders")
        self.cpp_info.components["vulkanutilityheaders"].bindirs = []
        self.cpp_info.components["vulkanutilityheaders"].libdirs = []
        self.cpp_info.components["vulkanutilityheaders"].includedirs = ["include"]
        self.cpp_info.components["vulkanutilityheaders"].requires = ["vulkan-headers::vulkanheaders"]
        self.cpp_info.components["vulkanutilityheaders"].libs = []

        self.cpp_info.components["vulkansafestruct"].set_property("cmake_target_name", "Vulkan::SafeStruct")
        self.cpp_info.components["vulkansafestruct"].bindirs = []
        self.cpp_info.components["vulkansafestruct"].libdirs = ["lib"]
        self.cpp_info.components["vulkansafestruct"].includedirs = ["include"]
        self.cpp_info.components["vulkansafestruct"].requires = ["vulkan-headers::vulkanheaders", "vulkanutilityheaders"]
        self.cpp_info.components["vulkansafestruct"].libs = ["VulkanSafeStruct"]

    def test(self):
        if can_run(self):
            bin_path = os.path.join(self.cpp.build.bindirs[0], "test_package")
            self.run(bin_path, env="conanrun")
