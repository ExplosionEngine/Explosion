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
        # DXC's StringRef.h specializes std::is_nothrow_constructible (an upstream "HLSL Change"). Newer Clang flags
        # specializing a standard library entity as -Winvalid-specialization, which is an error by default, so recent
        # Apple Clang fails to build the pinned source. Suppress the diagnostic (the specialization still takes effect,
        # preserving upstream behavior); -Wno-unknown-warning-option keeps older Clang that lacks the flag happy.
        if str(self.settings.compiler) in ("clang", "apple-clang"):
            cmake_toolchain.extra_cxxflags.extend(["-Wno-unknown-warning-option", "-Wno-invalid-specialization"])
        cmake_toolchain.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        install_folder = os.path.join(self.build_folder, "installed")
        predefined_params_file = os.path.join(self.source_folder, "cmake", "caches", "PredefinedParams.cmake")

        cmake = CMake(self)
        # Disable every test sub-tree:
        #   * HLSL_INCLUDE_TESTS=ON (set by PredefinedParams.cmake) drags in TAEF
        #     via projects/dxilconv/unittests, which isn't shipped with the SDK.
        #   * SPIRV_BUILD_TESTS=ON (also from PredefinedParams) builds GoogleTest.
        #   * LLVM/CLANG_INCLUDE_TESTS=ON (CMake default) wires `check-all` up to
        #     `ExecHLSLTests`/`dxc_batch`, which only exist when HLSL tests are on
        #     and break configure once we turn them off.
        # Command line -D values take precedence over the -C cache file.
        cmake.configure(cli_args=[
            "-C", predefined_params_file,
            f"-DCMAKE_INSTALL_PREFIX={install_folder}",
            "-DHLSL_INCLUDE_TESTS=OFF",
            "-DSPIRV_BUILD_TESTS=OFF",
            "-DLLVM_INCLUDE_TESTS=OFF",
            "-DCLANG_INCLUDE_TESTS=OFF",
        ])
        cmake.build()
        cmake.build(target="install-distribution")

    def package(self):
        copy(self, "*", os.path.join(self.build_folder, "installed", "include"), os.path.join(self.package_folder, "include"))
        copy(self, "*", os.path.join(self.build_folder, "installed", "bin"), os.path.join(self.package_folder, "bin"))
        if self.settings.os == "Windows":
            copy(self, "d3d12shader.h", os.path.join(self.source_folder, "external", "DirectX-Headers", "include", "directx"), os.path.join(self.package_folder, "include", "dxc"))
            copy(self, "dxil.dll", os.path.join(self.build_folder, "bin"), os.path.join(self.package_folder, "bin"))
            copy(self, "dxcompiler.lib", os.path.join(self.build_folder, "lib"), os.path.join(self.package_folder, "lib"))
        elif self.settings.os in ("Macos", "Linux"):
            # install-distribution drops the shared compiler (libdxcompiler.dylib /
            # libdxcompiler.so) under installed/lib on both Unix-likes.
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
