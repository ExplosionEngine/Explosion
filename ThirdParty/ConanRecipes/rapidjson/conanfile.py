from conan import ConanFile
from conan.tools.build import can_run
from conan.tools.scm import Git
from conan.tools.files import apply_conandata_patches, get, copy
import os

required_conan_version = ">=2.0.9"

class RapidJsonConan(ConanFile):
    name = "rapidjson"
    description = "rapidjson"
    license = "https://github.com/Tencent/rapidjson?tab=License-1-ov-file"
    url = "https://github.com/conan-io/conan-center-index"
    homepage = "https://github.com/Tencent/rapidjson"
    topics = ("utils", "json")
    package_type = "header-library"

    def layout(self):
        self.folders.source = "src"
        self.folders.build = "build"
        self.folders.generators = "build/generators"

    def source(self):
        git = Git(self)
        git.clone("https://github.com/Tencent/rapidjson.git", target=".")
        git.checkout(self.conan_data["sources"][self.version]["commit"])
        apply_conandata_patches(self)

    def package(self):
        copy(self, "*", os.path.join(self.source_folder, "include"), os.path.join(self.package_folder, "include"))

    def package_info(self):
        self.cpp_info.includedirs = ["include"]

    def test(self):
        if can_run(self):
            bin_path = os.path.join(self.cpp.build.bindirs[0], "test_package")
            self.run(bin_path, env="conanrun")
