from conan import ConanFile
from conan.tools.build import can_run
from conan.tools.files import get, apply_conandata_patches, copy, export_conandata_patches
import os

required_conan_version = ">=2.0.9"

class ClippConan(ConanFile):
    name = "clipp"
    description = "clipp"
    license = "https://github.com/muellan/clipp?tab=MIT-1-ov-file"
    url = "https://github.com/conan-io/conan-center-index"
    homepage = "https://github.com/muellan/clipp"
    topics = ("utils", "cli")
    package_type = "header-library"

    def export_sources(self):
        export_conandata_patches(self)

    def layout(self):
        self.folders.source = "src"
        self.folders.build = "build"
        self.folders.generators = "build/generators"

    def source(self):
        get(self, **self.conan_data["sources"][self.version], strip_root=True)
        apply_conandata_patches(self)

    def package(self):
        copy(self, "*.h", os.path.join(self.source_folder, "include"), os.path.join(self.package_folder, "include"))

    def package_info(self):
        self.cpp_info.includedirs = ["include"]

    def test(self):
        if can_run(self):
            bin_path = os.path.join(self.cpp.build.bindirs[0], "test_package")
            self.run(bin_path, env="conanrun")
