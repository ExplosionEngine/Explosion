
<center>
    <div><img width="800" src=".github/resource/Logo.png" alt="Explosion Logo"/></div>
    <div>
        <a href="https://discord.gg/Tn5G3ReYhD">
            <img src="https://img.shields.io/discord/852860169045278720?style=for-the-badge" alt="Discord"/>
        </a>
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

Explosion is a cross-platform game engine, based on modern graphics api (aka Vulkan, DirectX 12, Metal). Welcome to join the next generation game order.

# Requirements

There are some supported compilers yet:

| OS | Compiler |
| - | - |
| Windows | Visual Studio 16 2019 x64 |

# Build The Engine

Using cmake to configure and build project:

```cpp
# Debug
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug

# Release
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

# Sponsor

<img width="200dp" src=".github/resource/JetBrains.png" alt="JetBrains Open Source"/>

# License

[MIT](https://github.com/ExplosionEngine/Explosion/blob/master/LICENSE) @ Explosion Development Team All right Reserved 2022.
