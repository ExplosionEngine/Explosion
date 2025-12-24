from conan import ConanFile
from conan.tools.build import check_min_cppstd, can_run
from conan.tools.cmake import cmake_layout, CMakeToolchain, CMakeDeps, CMake
from conan.tools.files import apply_conandata_patches
from conan.tools.scm import Git
import os

required_conan_version = ">=2.0.9"

class GlfwConan(ConanFile):
    name = "glfw"
    description = "glfw window library"
    license = "https://github.com/glfw/glfw?tab=Zlib-1-ov-file"
    url = "https://github.com/conan-io/conan-center-index"
    homepage = "https://github.com/glfw/glfw"
    topics = ("opengl", "vulkan", "window-management")
    package_type = "static-library"
    settings = "os", "arch", "compiler", "build_type"
    options = {}
    default_options = {}

    def layout(self):
        cmake_layout(self, src_folder="src")

    def validate(self):
        check_min_cppstd(self, 17)

    def build_requirements(self):
        self.tool_requires("ninja/[>=1.12]")
        self.tool_requires("cmake/[>=3.16]")

    def source(self):
        git = Git(self)
        git.clone("https://github.com/glfw/glfw", target=".")
        git.checkout(self.conan_data["sources"][self.version]["commit"])
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
        self.cpp_info.set_property("cmake_file_name", "glfw")
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.libs = ["glfw3"]
        self.cpp_info.libdirs = ["lib"]

    def test(self):
        if can_run(self):
            bin_path = os.path.join(self.cpp.build.bindirs[0], "test_package")
            self.run(bin_path, env="conanrun")
