
<center>
    <div><img width="800" src="./Docs/Img/logo.png" alt="Explosion Logo"/></div>
    <div>
        <img src="https://img.shields.io/github/workflow/status/ExplosionEngine/Explosion/CMake?style=for-the-badge" alt="Build Status"/>
        <img src="https://img.shields.io/github/commit-activity/m/ExplosionEngine/Explosion?style=for-the-badge" alt="Commit Activity"/>
        <img src="https://img.shields.io/github/license/ExplosionEngine/Explosion?style=for-the-badge" alt="License"/>
        <img src="https://img.shields.io/github/languages/code-size/ExplosionEngine/Explosion?style=for-the-badge" alt="Code Size"/>
    </div>
</center>

# Introduction

Explosion is a cross-platform game engine, based on modern graphics api (aka Vulkan, DirectX 12, Metal) and powered by advanced logical framework called ECS.

Welcome to join next generation game order.

# Build The Engine

Install [Vulkan SDK](https://vulkan.lunarg.com/sdk/home) and setup environment variable:

```cpp
VULKAN_SDK=${your_path}
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

# About

Explosion development team, all right reserved.
