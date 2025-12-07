
<div>
    <div><img width="400" src=".github/resource/Logo.png" alt="Explosion Logo"/></div>
    <div>
        <a href="https://app.codacy.com/gh/ExplosionEngine/Explosion/dashboard?branch=master">
            <img src="https://img.shields.io/codacy/grade/98afe27fd39b4b39b4c6acd8361e6d02?style=for-the-badge" alt="Codacy Grade"/>
        </a>
        <a href="https://github.com/ExplosionEngine/Explosion/blob/master/LICENSE">
            <img src="https://img.shields.io/github/license/ExplosionEngine/Explosion?style=for-the-badge" alt="License"/>
        </a>
        <img src="https://img.shields.io/github/commit-activity/m/ExplosionEngine/Explosion?style=for-the-badge" alt="Commit Activity"/>
    </div>
</div>

# Introduction

Explosion is a cross-platform C++ game engine, based on modern graphics api (aka Vulkan, DirectX 12, Metal), powered by ECS gameplay framework.

# Requirements

You need install those tools to system by yourself and add them to path:

* [Python3](https://www.python.org/downloads/)
* [Conan](https://github.com/conan-io/conan)
* [CMake](https://cmake.org/download/)
* [Ninja](https://github.com/ninja-build/ninja)
* [Node.js](https://nodejs.org/en/download)

build steps of engine may use them.

# Build

## Configure Conan

Some third-party libraries managed by Conan may need to be downloaded and installed from our private repository. Please configure Conan's remote first:

```shell
conan remote add explosion https://kindem.online/artifactory/api/conan/conan
```

Additionally, to ensure the qt-webengine module builds successfully, you need to install html5lib in python:

```shell
pip3 install html5lib
```

## macOS Notice
If you have not installed xcode and xcode command line tool, you need install them, xcode can be downloaded from app store, and xcode command line tool can be installed with:

```shell
xcode-select --install
```

In cmake configure stage, if package with your compiler and os info not be cached in our repo, conan will build qt locally, which need set xcode path to app path to make qt build success:

```cpp
sudo xcode-select -s /Applications/Xcode.app/Contents/Developer
```

But in explosion project build stage, we use command line tool path instead, so you need switch it:

```shell
sudo xcode-select -s /Library/Developer/CommandLineTools
```

## Windows Notice

Some third-party libraries managed by conan may fail to install or compile on Windows due to excessively long build tree paths. So you need to configure the environment variable `CONAN_HOME` and set it to a relatively short path, such as `C:\t`

## Build Project

The following table contains supported platform, toolchain and generator:

<table>
    <tr>
        <td>Platform</td>
        <td>Arch</td>
        <td>Toolchain</td>
        <td>Generator</td>
    </tr>
    <tr>
        <td rowspan="4">Windows</td>
        <td rowspan="4">x64</td>
        <td rowspan="4">Visual Studio</td>
        <td>Visual Studio 2019</td>
    </tr>
    <tr>
        <td>Visual Studio 2022</td>
    </tr>
    <tr>
        <td>Ninja</td>
    </tr>
    <tr>
        <td>Ninja Multi-Config</td>
    </tr>
    <tr>
        <td rowspan="3">macOS</td>
        <td rowspan="3">arm64</td>
        <td rowspan="3">Apple Clang</td>
        <td>Unix Makefiles</td>
    </tr>
    <tr>
        <td>Ninja</td>
    </tr>
    <tr>
        <td>Ninja Multi-Config</td>
    </tr>
</table>

By cause of cmake, we recommend [CLion](https://www.jetbrains.com/clion/) as Explosion's IDE, which can help you build and manage project simplely, and brings best coding experience to you. When use CLion as IDE, you just need open the project and configure cmake toolchain and generator in settings, press build and everything is down.

If you want to continue with command line mode, the following commands may help:

```cpp
# for single config generator, such as Ninja, Unix Makefiles
cmake -S . -B <build_dir> -G <generator_name> -DCMAKE_BUILD_TYPE=<build_config>
cmake --build <build_dir>

# for multi config generator, such as Visual Studio, Ninja Multi Config
cmake -S . -B <build_dir> -G <generator_name>
cmake --build <build_dir> --config <build_config>
```

The parameters' meaning:
* `<build_dir>`: Build directory for saving cmake temporal files and dist binaries. You can transfer a `build` or any directory you like.
* `<build_config>`: Build config of cmake, such as `Debug` or `Release`.
* `<generator_name>`: C++ generator which cmake will use to build project, you can transfer any generator in table list above, such as `Visual Studio 2019`.

After build the project, you can get all dist binaries in `<build_dir>/dist`.

# Third Party Project Usage

Thanks all those following projects:

* [Conan](https://github.com/conan-io/conan)
* [DirectX-Headers](https://github.com/microsoft/DirectX-Headers)
* [Vulkan](https://www.vulkan.org/)
* [DirectXShaderCompiler](https://github.com/microsoft/DirectXShaderCompiler)
* [GLFW](https://www.glfw.org/)
* [clipp](https://github.com/muellan/clipp)
* [debugbreak](https://github.com/scottt/debugbreak)
* [LLVM](https://llvm.org/)
* [googletest](https://github.com/google/googletest)
* [taskflow](https://github.com/taskflow/taskflow)
* [cityhash](https://github.com/google/cityhash)
* [stb](https://github.com/nothings/stb)
* [fmt](https://github.com/fmtlib/fmt)
* [SPIRV-Cross](https://github.com/KhronosGroup/SPIRV-Cross)
* [assimp](https://github.com/assimp/assimp)
* [VulkanMemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
* [Qt6](https://www.qt.io/product/qt6)
* [rapidjson](https://github.com/Tencent/rapidjson)
* [cpp-httplib](https://github.com/yhirose/cpp-httplib)
* [React](https://react.dev)
* [Vite](https://vite.dev)
* [HeroUI](https://www.heroui.com)
* [TailWindCSS](https://tailwindcss.com)

# Sponsor

<img width="100dp" src=".github/resource/JetBrains.png" alt="JetBrains Open Source"/>

# License

[MIT](https://github.com/ExplosionEngine/Explosion/blob/master/LICENSE) @ Explosion Development Team All right Reserved 2025.
