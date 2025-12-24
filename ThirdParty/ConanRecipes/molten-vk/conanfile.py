import re

from conan import ConanFile
from conan.tools.build import check_min_cppstd, can_run
from conan.tools.cmake import cmake_layout, CMakeToolchain, CMakeDeps, CMake
from conan.tools.files import apply_conandata_patches, copy
from conan.tools.scm import Git
import os
import shutil

required_conan_version = ">=2.0.9"

class MoltenVKConan(ConanFile):
    name = "molten-vk"
    description = "molten vk (vulkan driver on macos)"
    license = "https://github.com/KhronosGroup/MoltenVK?tab=Apache-2.0-1-ov-file"
    url = "https://github.com/conan-io/conan-center-index"
    homepage = "https://github.com/KhronosGroup/MoltenVK"
    topics = ("vulkan", "driver", "macos")
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
        git.clone("https://github.com/KhronosGroup/MoltenVK", target=".")
        git.checkout(self.conan_data["sources"][self.version]["commit"])
        apply_conandata_patches(self)

    def generate(self):
        cmake_toolchain = CMakeToolchain(self, generator="Ninja")
        cmake_toolchain.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        install_folder = os.path.join(self.build_folder, "installed")
        cmake.configure(cli_args=[f"-DCMAKE_INSTALL_PREFIX={install_folder}"])
        cmake.build()
        cmake.install(cli_args=["--prefix", install_folder])

        lib_root = os.path.join(install_folder, "lib")
        for entry in os.listdir(lib_root):
            entry_path = os.path.join(lib_root, entry)
            if os.path.isdir(entry_path):
                shutil.rmtree(entry_path)
            elif os.path.islink(entry_path):
                os.remove(entry_path)
            elif entry.startswith("libMoltenVK"):
                os.rename(entry_path, os.path.join(lib_root, "libMoltenVK.dylib"))

        icd_descriptor_json = os.path.join(install_folder, "etc", "vulkan", "icd.d", "MoltenVK_icd.json")
        new_icd_descriptor_json = os.path.join(install_folder, "lib", "MoltenVK_icd.json")
        with open(icd_descriptor_json, "r") as file:
            content = file.read()
        new_content = re.sub(r"\"library_path\": \".+\"", "\"library_path\": \"libMoltenVK.dylib\"", content)
        with open(new_icd_descriptor_json, "w") as file:
            file.write(new_content)

    def package(self):
        install_folder = os.path.join(self.build_folder, "installed")
        copy(self, "*", install_folder, self.package_folder)
        os.mkdir(os.path.join(self.package_folder, "include"))

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "MoltenVK")
        self.cpp_info.bindirs = ["bin"]
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.libs = []
        self.cpp_info.libdirs = []

    def test(self):
        if can_run(self):
            bin_path = os.path.join(self.cpp.build.bindirs[0], "test_package")
            self.run(bin_path, env="conanrun")
