# ConanRecipes

This repository stores all private Conan recipes used by the explosion game engine. Typically, you don't need to build them separately, as all recipes and precompiled binaries have been uploaded to our private artifact repository at [https://conan.kindem.online/artifactory/api/conan/conan](https://conan.kindem.online/artifactory/api/conan/conan)
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

To build every recipe at once, use the `build_recipes.py` helper. It walks each
recipe directory, picks the latest version (the top-most entry in
`conandata.yml`) and builds them one-by-one in dependency order. Each recipe
lists the platforms it supports under a `platforms` key in its `conandata.yml`
(currently `Windows-x86_64` and/or `Macos-armv8`); recipes that do not target
the current host are skipped. If any recipe fails the script stops immediately
and prints a summary.

```shell
cd ThirdParty/ConanRecipes
# build every recipe for the current host
python build_recipes.py

# build everything, then upload to a remote (upload only runs if every
# recipe built successfully)
python build_recipes.py --upload \
    --remote <remote> \
    --remote-url <remote-url> \
    --remote-user <user> --remote-password <password>

# just register every recipe version in the local conan cache (no build);
# a later 'conan install --build=missing' then builds whatever the remote
# has no binaries for
python build_recipes.py --export-only
```

## Windows notes

Some recipes build code that needs ATL (e.g. `atlbase.h`), which a stock Visual
Studio C++ installation does not ship. If a build fails with a missing-ATL
error, open the Visual Studio Installer and add the **C++ ATL** component
(Modify -> Individual components -> "C++ ATL for latest build tools").

## CI

Recipe changes are validated and published automatically:

- Every pull request runs the engine build workflow, which first runs
  `build_recipes.py --export-only`. Unchanged recipes hash to the same
  revision already published on the remote so their binaries are simply
  downloaded, while recipes changed by the PR are built from source inside
  the job. A PR can therefore change recipes and engine code together and
  be validated atomically.
- After a push to `master` that touches `ThirdParty/ConanRecipes`, the
  `Publish Conan Recipes` workflow builds the changed recipes on Windows
  and macOS and uploads them to the remote (credentials come from the
  `CONAN_REMOTE_USER` / `CONAN_REMOTE_PASSWORD` repository secrets). It can
  also be re-run manually via `workflow_dispatch` if an upload failed.

To keep remote revisions in sync with git, avoid uploading from local
machines; let the publish workflow be the only writer.
