from conan import ConanFile
from conan.tools.build import check_min_cppstd
from conan.tools.build import can_run
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import apply_conandata_patches, get, copy
from pip._internal import main as pip_main
import os

required_conan_version = ">=2.0.9"

class QtConan(ConanFile):
    name = "qt"
    description = "qt"
    license = "https://github.com/qt/qt5/tree/dev/LICENSES"
    url = "https://github.com/qt/qt5"
    homepage = "https://github.com/qt/qt5"
    topics = ("gui", "tool")
    package_type = "shared-library"
    settings = "os", "arch", "compiler", "build_type"
    options = {}
    default_options = {}

    __enabled_projects = (
        "qtbase", "qtdeclarative", "qttools", "qtshadertools", "qtpositioning", "qtwebchannel", "qtwebengine", "qtwebsockets"
    )

    def layout(self):
        cmake_layout(self, src_folder="src")

    def validate(self):
        check_min_cppstd(self, 17)

    def build_requirements(self):
        self.tool_requires("ninja/[>=1.12]")
        self.tool_requires("cmake/[>=3.16]")

        # for qtwebengine
        self.tool_requires("nodejs/22.20.0")
        self.tool_requires("gperf/3.1")
        if self.settings.os == "Windows":
            self.tool_requires("winflexbison/2.5.25")
        else:
            self.tool_requires("bison/3.8.2")
            self.tool_requires("flex/2.6.4")

    def source(self):
        get(self, **self.conan_data["sources"][self.version], strip_root=True)
        apply_conandata_patches(self)

    def generate(self):
        # for qtwebengine
        print("enabled projects contains qtwebengine, checking html5lib installed...")
        try:
            import html5lib
            print("html5lib import test is OK")
        except ImportError:
            print("html5lib not installed, try pip install")
            if self.settings.os == "Windows":
                pip_main(["install", "html5lib"])
            else:
                pip_main(["install", "html5lib", "--break-system-packages"])
        try:
            import html5lib
        except ImportError:
            raise RuntimeError("failed to import html5lib")

        # to ensure qtwebengine build success
        if self.settings.os != "Windows":
            import resource
            soft_limit, hard_limit = resource.getrlimit(resource.RLIMIT_NOFILE)
            resource.setrlimit(resource.RLIMIT_NOFILE, (1000000, hard_limit))
            soft_limit, hard_limit = resource.getrlimit(resource.RLIMIT_NOFILE)
            print(f"set resource.RLIMIT_NOFILE for qtwebengine build to (soft: {soft_limit}, hard: {hard_limit})")

        cmake_toolchain = CMakeToolchain(self, generator="Ninja")
        cmake_toolchain.cache_variables["QT_AUTODETECT_ANDROID"] = ""
        cmake_toolchain.cache_variables["QT_GENERATE_SBOM"] = "OFF"
        for entry in os.listdir(self.source_folder):
            if not os.path.isdir(entry) and not entry.startswith('qt'):
                continue
            cmake_toolchain.cache_variables[f"BUILD_{entry}"] = "ON" if entry in self.__enabled_projects else "OFF"
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
        self.cpp_info.libs = []
        self.cpp_info.builddirs = [os.path.join("lib", "cmake")]
        self.cpp_info.set_property("cmake_find_mode", "none")

    def test(self):
        if can_run(self):
            bin_path = os.path.join(self.cpp.build.bindirs[0], "test_package")
            self.run(bin_path, env="conanrun")
