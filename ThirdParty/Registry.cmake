if (${USE_CONAN})
    # Deploy the dependencies into the install tree so downstream projects (which take the else() branch below) can
    # find_package() them. This must reuse the very host profile cmake-conan generated for the engine build: that
    # profile pins compiler.cppstd to CMAKE_CXX_STANDARD, so resolving with the bare default profile instead would
    # compute different package ids and fail the deploy - either as "invalid packages" where the default cppstd is
    # lower than a dependency requires, or as a "missing binary" mismatch against the binaries already in the cache.
    install(CODE "execute_process(COMMAND conan install ${CMAKE_SOURCE_DIR} --profile:host=default --profile:host=${CMAKE_BINARY_DIR}/conan_host_profile --profile:build=default -s build_type=Release -c tools.cmake.cmakedeps:new=will_break_next --deployer=full_deploy --output-folder=${CMAKE_INSTALL_PREFIX}/${SUB_PROJECT_NAME}/ThirdParty COMMAND_ERROR_IS_FATAL ANY)")
else ()
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${CMAKE_CURRENT_LIST_DIR}/../../ThirdParty")
endif ()

find_package(httplib REQUIRED GLOBAL)
find_package(glfw3 REQUIRED GLOBAL)
find_package(stb REQUIRED GLOBAL)
find_package(cityhash REQUIRED GLOBAL)
find_package(GTest REQUIRED GLOBAL)
find_package(benchmark REQUIRED GLOBAL)
find_package(Taskflow REQUIRED GLOBAL)
find_package(libclang REQUIRED GLOBAL)
find_package(assimp REQUIRED GLOBAL)
find_package(VulkanMemoryAllocator REQUIRED GLOBAL)
find_package(debugbreak REQUIRED GLOBAL)
find_package(RapidJSON REQUIRED GLOBAL)
find_package(clipp REQUIRED GLOBAL)
find_package(dxc REQUIRED GLOBAL)
find_package(VulkanHeaders REQUIRED GLOBAL)
find_package(VulkanLoader REQUIRED GLOBAL)
find_package(vulkan-validationlayers REQUIRED GLOBAL)
find_package(spirv-cross REQUIRED GLOBAL)

# The official vulkan-validationlayers package only ships a runtime layer with no headers or link libraries, so the
# generator declares no usable target. Recreate it as an imported target so it can carry the RUNTIME_DEP layer files
# and still be linked by RHI-Vulkan the same way as before.
if (NOT TARGET vulkan-validationlayers::vulkan-validationlayers)
    add_library(vulkan-validationlayers::vulkan-validationlayers INTERFACE IMPORTED GLOBAL)
endif ()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    find_package(DirectX-Headers REQUIRED GLOBAL)
    set_target_properties(assimp::assimp PROPERTIES RUNTIME_DEP "${assimp_INCLUDE_DIR}/../bin/assimp-vc${MSVC_TOOLSET_VERSION}-mt.dll")
    set_target_properties(libclang::libclang PROPERTIES RUNTIME_DEP "${libclang_INCLUDE_DIR}/../bin/libclang.dll")
    set_target_properties(dxc::dxc PROPERTIES RUNTIME_DEP "${dxc_INCLUDE_DIR}/../bin/dxil.dll;${dxc_INCLUDE_DIR}/../bin/dxcompiler.dll")
    set_target_properties(vulkan-validationlayers::vulkan-validationlayers PROPERTIES RUNTIME_DEP "${vulkan-validationlayers_PACKAGE_FOLDER_RELEASE}/bin/VkLayer_khronos_validation.dll;${vulkan-validationlayers_PACKAGE_FOLDER_RELEASE}/bin/VkLayer_khronos_validation.json")
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    find_package(MoltenVK REQUIRED GLOBAL)
    set_target_properties(vulkan-validationlayers::vulkan-validationlayers PROPERTIES RUNTIME_DEP "${vulkan-validationlayers_PACKAGE_FOLDER_RELEASE}/lib/libVkLayer_khronos_validation.dylib;${vulkan-validationlayers_PACKAGE_FOLDER_RELEASE}/res/vulkan/explicit_layer.d/VkLayer_khronos_validation.json")
    set_target_properties(molten-vk::molten-vk PROPERTIES RUNTIME_DEP "${MoltenVK_INCLUDE_DIR}/../lib/libMoltenVK.dylib;${MoltenVK_INCLUDE_DIR}/../lib/MoltenVK_icd.json")
endif ()
