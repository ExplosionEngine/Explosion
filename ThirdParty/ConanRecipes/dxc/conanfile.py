from conan import ConanFile
from conan.tools.build import check_min_cppstd, can_run
from conan.tools.cmake import cmake_layout, CMakeToolchain, CMakeDeps, CMake
from conan.tools.files import apply_conandata_patches, copy
from conan.tools.scm import Git
import os

required_conan_version = ">=2.0.9"

class DXCConan(ConanFile):
    name = "dxc"
    description = "DirectX shader compiler"
    license = "https://github.com/microsoft/DirectXShaderCompiler?tab=License-1-ov-file"
    url = "https://github.com/conan-io/conan-center-index"
    homepage = "https://github.com/microsoft/DirectXShaderCompiler"
    topics = ("shader", "tool", "compiler")
    package_type = "shared-library"
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
        git.clone("https://github.com/microsoft/DirectXShaderCompiler", target=".")
        git.checkout(self.conan_data["sources"][self.version]["commit"])
        git.run("submodule update --init --recursive")
        apply_conandata_patches(self)

    def generate(self):
        cmake_toolchain = CMakeToolchain(self, generator="Ninja")
        cmake_toolchain.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        install_folder = os.path.join(self.build_folder, "installed")
        predefined_params_file = os.path.join(self.source_folder, "cmake", "caches", "PredefinedParams.cmake")

        cmake = CMake(self)
        cmake.configure(cli_args=["-C", predefined_params_file, f"-DCMAKE_INSTALL_PREFIX={install_folder}"])
        cmake.build()
        cmake.build(target="install-distribution")

    def package(self):
        copy(self, "*", os.path.join(self.build_folder, "installed", "include"), os.path.join(self.package_folder, "include"))
        copy(self, "*", os.path.join(self.build_folder, "installed", "bin"), os.path.join(self.package_folder, "bin"))
        if self.settings.os == "Windows":
            copy(self, "d3d12shader.h", os.path.join(self.source_folder, "external", "DirectX-Headers", "include", "directx"), os.path.join(self.package_folder, "include", "dxc"))
            copy(self, "dxil.dll", os.path.join(self.build_folder, "bin"), os.path.join(self.package_folder, "bin"))
            copy(self, "dxcompiler.lib", os.path.join(self.build_folder, "lib"), os.path.join(self.package_folder, "lib"))
        elif self.settings.os == "Macos":
            copy(self, "*", os.path.join(self.build_folder, "installed", "lib"), os.path.join(self.package_folder, "lib"))

    def package_info(self):
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.libdirs = ["lib"]
        self.cpp_info.bindirs = ["bin"]
        self.cpp_info.libs = ["dxcompiler"]

    def test(self):
        if can_run(self):
            bin_path = os.path.join(self.cpp.build.bindirs[0], "test_package")
            self.run(bin_path, env="conanrun")
