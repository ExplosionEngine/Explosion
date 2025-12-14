from conan import ConanFile

class ExplosionConan(ConanFile):
    generators = "CMakeConfigDeps"
    settings = "os", "compiler", "build_type", "arch"

    def requirements(self):
        self.requires("cpp-httplib/0.27.0")
        self.requires("glfw/3.4")
        self.requires("stb/cci.20230920")
        self.requires("cityhash/1.0.1")
        self.requires("gtest/1.17.0")
        self.requires("taskflow/3.10.0")
        self.requires("vulkan-memory-allocator/3.3.0")
        self.requires("assimp/6.0.2", options={
            "shared": True
        })

        if self.settings.os == "Windows":
            self.requires("directx-headers/1.610.2")

        # private repo
        self.requires("libclang/21.1.7-exp")
        self.requires("qt/6.10.1-exp")
        self.requires("debugbreak/1.0-exp")
