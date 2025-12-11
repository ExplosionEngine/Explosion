# ConanRecipes

This repository stores all private Conan recipes used by the explosion game engine. Typically, you don't need to build them separately, as all recipes and precompiled binaries have been uploaded to our private artifact repository at [https://kindem.online/artifactory/api/conan/conan](https://kindem.online/artifactory/api/conan/conan)
. The CMake scripts in the explosion game engine will automatically download and install all dependencies from this repository.

If you have extremely poor network connectivity, you may use these Conan recipes to locally build the required third-party dependencies for the engine.

Here is a simple example:

```shell
cd ThirdParty/ConanRecipes
conan create qt/conanfile.py --version="6.10.1-exp"
```

For explosion engine developers, those commands may help to debug conan recipes:

```shell
cd ThirdParty/ConanRecipes
# source stage
conan source qt/conanfile.py --version="6.10.1-exp"
# build stage
conan build qt/conanfile.py --version="6.10.1-exp"
# export stage
conan export-pkg qt/conanfile.py --version="6.10.1-exp"
# test stage
conan test qt/test_package qt/6.10.1-exp
```

# Windows User Notice
In windows, some lib may build failed with long build tree path in development mode (like qt-webengine), in this case, you can use the commands to map conan recipes working directory as a driver and execute all conan commands in the driver root:

```shell
# map
subst z: path/to/engine/ThirdParty/ConanRecipes

# unmap
subst z: /d
```
