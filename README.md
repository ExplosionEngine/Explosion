
<center>
    <div><img width="800" src="./Docs/Img/logo.png" alt="Explosion Logo"/></div>
    <div>
        <img src="https://img.shields.io/github/workflow/status/ExplosionEngine/Explosion/CMake?style=for-the-badge" alt="Build Status"/>
        <img src="https://img.shields.io/github/commit-activity/m/ExplosionEngine/Explosion?style=for-the-badge" alt="Commit Activity"/>
        <img src="https://img.shields.io/github/license/ExplosionEngine/Explosion?style=for-the-badge" alt="License"/>
        <img src="https://img.shields.io/github/languages/code-size/ExplosionEngine/Explosion?style=for-the-badge" alt="Code Size"/>
    </div>
</center>
---

Table of Contents
=================


   * [Catalog](#catalog)
   * [Introduction](#introduction)
   * [Change Log](#change-log)
         * [v0.0.1 ( 2021/0X/0X )](#v001--20210x0x-)
   * [Usage](#usage)
      * [1. Build Game Engine](#1-build-game-engine)
   * [License](#license)
   * [URL](#url)

<br>

# Introduction

---

**Explosion** is a cross-platform game engine, based on modern **Graphic APIs** (aka Vulkan, DirectX 12, Metal) and powered by advanced logical framework called ECS.

Welcome to join the next generation game order.

<br>

# Change Log

---

### v0.0.1 ( 2021/0X/0X )

+ 
+ 

<br>

# Usage

---

## 1. Build Game Engine

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

<br>

# License

---

[MIT](https://github.com/ExplosionEngine/Explosion/blob/master/LICENSE) @ Explosion Development Team All right Reserved 2021.

<br>

# URL

---

Repository Page: https://github.com/ExplosionEngine/Explosion

Wiki Page:  https://github.com/ExplosionEngine/Explosion/wiki

