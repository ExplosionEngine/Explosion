option(ENABLE_3RD_DEBUG_INFO "3rd package debug" ON)
option(CUSTOM_3RD_REPO "using custom 3rd repo" OFF)

if (${CUSTOM_3RD_REPO})
    set(3RD_REPO ${3RD_REPO_URL} CACHE STRING "" FORCE)
else()
    set(3RD_REPO "http://124.223.92.162" CACHE STRING "" FORCE)
endif()

set(3RD_DIR ${CMAKE_SOURCE_DIR}/ThirdParty CACHE PATH "" FORCE)
set(3RD_ZIP_DIR ${3RD_DIR}/Zip CACHE PATH "" FORCE)
set(3RD_SOURCE_DIR ${3RD_DIR}/Lib CACHE PATH "" FORCE)
set(3RD_BINARY_DIR ${CMAKE_BINARY_DIR}/ThirdPartyBuild CACHE PATH "" FORCE)
set(3RD_INSTALL_DIR ${CMAKE_BINARY_DIR}/ThirdPartyInstall CACHE PATH "" FORCE)

# AddThirdPartyPackage
# Description: add a third party package
# Params:
#  - NAME     {Single} : name of third party package
#  - VERSION  {Single} : version of third party package
#  - ARG      {List}   : arguments of cmake command
#  - BUILD    {Bool}   : build package or not (just using sources)
#  - PLATFORM {Bool}   : true if package is platform relative
function(AddThirdPartyPackage)
    cmake_parse_arguments(PARAMS "BUILD;PLATFORM" "NAME;VERSION" "ARG" ${ARGN})

    set(3RD_PACKAGE_NAME "${PARAMS_NAME}")
    if (${PARAMS_PLATFORM})
        set(3RD_PACKAGE_FULL_NAME "${PARAMS_NAME}-${CMAKE_SYSTEM_NAME}-${PARAMS_VERSION}")
    else()
        set(3RD_PACKAGE_FULL_NAME "${PARAMS_NAME}-${PARAMS_VERSION}")
    endif()
    set(3RD_PACKAGE_URL "${3RD_REPO}/${3RD_PACKAGE_FULL_NAME}.zip")
    set(3RD_PACKAGE_SOURCE_DIR "${3RD_SOURCE_DIR}/${3RD_PACKAGE_FULL_NAME}")

    if (${PARAMS_BUILD})
        set(3RD_PACKAGE_BINARY_DIR "${3RD_BINARY_DIR}/${3RD_PACKAGE_NAME}")
        set(3RD_PACKAGE_INSTALL_DIR "${3RD_INSTALL_DIR}/${3RD_PACKAGE_NAME}/$<CONFIG>")
    endif()

    if (${ENABLE_3RD_DEBUG_INFO})
        message("")
        message("[3rd Package]")
        message(" - name: ${PARAMS_NAME}")
        message(" - version: ${PARAMS_VERSION}")
        message(" - platform: ${PARAMS_PLATFORM}")
        message(" - full name: ${3RD_PACKAGE_FULL_NAME}")
        message(" - url: ${3RD_PACKAGE_URL}")
        message("")
    endif()

    if (${PARAMS_BUILD})
        ExternalProject_Add(
            ${3RD_PACKAGE_NAME}
            URL ${3RD_PACKAGE_URL}
            DOWNLOAD_DIR ${3RD_ZIP_DIR}
            SOURCE_DIR ${3RD_PACKAGE_SOURCE_DIR}
            BINARY_DIR ${3RD_PACKAGE_BINARY_DIR}
            CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${3RD_PACKAGE_INSTALL_DIR} ${PARAMS_ARG}
            BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config $<CONFIG>
            INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR> --config $<CONFIG>
        )
    else()
        ExternalProject_Add(
            ${3RD_PACKAGE_NAME}
            URL ${3RD_PACKAGE_URL}
            DOWNLOAD_DIR ${3RD_ZIP_DIR}
            SOURCE_DIR ${3RD_PACKAGE_SOURCE_DIR}
            BINARY_DIR ${3RD_PACKAGE_BINARY_DIR}
            CMAKE_ARGS ""
            BUILD_COMMAND ""
            INSTALL_COMMAND ""
        )
    endif()

    set(${3RD_PACKAGE_NAME}_SOURCE_DIR ${3RD_PACKAGE_SOURCE_DIR} CACHE PATH "" FORCE)
    if (${PARAMS_BUILD})
        set(${3RD_PACKAGE_NAME}_BINARY_DIR ${3RD_PACKAGE_BINARY_DIR} CACHE PATH "" FORCE)
        set(${3RD_PACKAGE_NAME}_INSTALL_DIR ${3RD_PACKAGE_INSTALL_DIR} CACHE PATH "" FORCE)
    endif()
endfunction()
