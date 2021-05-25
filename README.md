
<center>
    <div><img width="800" src="./Docs/Img/logo.png" alt="Explosion Logo"/></div>
    <div>
        <a href="https://app.codacy.com/gh/ExplosionEngine/Explosion/dashboard?branch=master">
            <img src="https://img.shields.io/codacy/grade/98afe27fd39b4b39b4c6acd8361e6d02?style=for-the-badge" alt="Codacy Grade"/>
        </a>
        <a href="https://github.com/ExplosionEngine/Explosion/actions">
            <img src="https://img.shields.io/github/workflow/status/ExplosionEngine/Explosion/CMake?style=for-the-badge" alt="Build Status"/>
        </a>
        <a href="https://github.com/ExplosionEngine/Explosion/blob/master/LICENSE">
            <img src="https://img.shields.io/github/license/ExplosionEngine/Explosion?style=for-the-badge" alt="License"/>
        </a>
        <img src="https://img.shields.io/github/commit-activity/m/ExplosionEngine/Explosion?style=for-the-badge" alt="Commit Activity"/>
        <img src="https://img.shields.io/github/languages/code-size/ExplosionEngine/Explosion?style=for-the-badge" alt="Code Size"/>
    </div>
</center>

# Introduction

Explosion is a cross-platform game engine, based on modern graphics api (aka Vulkan, DirectX 12, Metal) and powered by advanced logical framework called ECS.

Welcome to join the next generation game order.

# Build The Engine

Clone the repository at first:

```shell
git clone --recurse-submodules https://github.com/ExplosionEngine/Explosion.git
```

If you already cloned the repository but has not updated sub-modules, please update them:

```shell
git submodule init
git submodule update
```

Install [Vulkan SDK](https://vulkan.lunarg.com/sdk/home) and setup environment variable:

```cpp
VULKAN_SDK=${your_sdk_path}
```

Using cmake to generate build rules:

```cpp
mkdir build
cd build
cmake ..
```

Building the engine:

```cpp
cmake --build .
```
# Documentation

[placeholder]

# Examples

[placeholder]

# License

[MIT](https://github.com/ExplosionEngine/Explosion/blob/master/LICENSE) @ Explosion Development Team All right Reserved 2021.
