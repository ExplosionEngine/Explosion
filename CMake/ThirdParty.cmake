set(3RD_DIR ${CMAKE_SOURCE_DIR}/ThirdParty CACHE PATH "" FORCE)
set(3RD_ZIP_DIR ${3RD_DIR}/Zip CACHE PATH "" FORCE)
set(3RD_SOURCE_DIR ${3RD_DIR}/Lib CACHE PATH "" FORCE)
set(3RD_BINARY_DIR ${CMAKE_BINARY_DIR}/ThirdPartyBuild CACHE PATH "" FORCE)
set(3RD_INSTALL_DIR ${CMAKE_BINARY_DIR}/ThirdPartyInstall CACHE PATH "" FORCE)

# ExtractZip
# Description: extract zip file
# Params:
#  - SRC {Single} : source zip file
#  - DST {Single} : output dir
function(ExtractZip)
    cmake_parse_arguments(PARAMS "" "SRC;DST" "" ${ARGN})

    if (NOT EXISTS ${PARAMS_SRC})
        message(FATAL_ERROR "Failed to found zip file: ${PARAMS_SRC}")
    endif()
    if (EXISTS ${PARAMS_DST})
        message("Found extracted third party package: ${PARAMS_DST}")
        return()
    endif()
    message("Starting extract zip file ${PARAMS_SRC}")

    if (${WIN32})
        execute_process(
            COMMAND powershell Expand-Archive -Path ${PARAMS_SRC} -DestinationPath ${PARAMS_DST}
        )
    else()
        execute_process(
            COMMAND unzip -n ${PARAMS_SRC} -d ${PARAMS_DST}
        )
    endif()
endfunction()

# AddThirdPartyPackage
# Description: add a third party package
# Params:
#  - NAME    {Single} : name of third party package
#  - VERSION {Single} : version of third party package
#  - ARG     {List}   : arguments of cmake command
#  - BUILD   {Bool}   : build package or not (just using sources)
function(AddThirdPartyPackage)
    cmake_parse_arguments(PARAMS "BUILD" "NAME;VERSION" "ARG" ${ARGN})

    set(3RD_PACKAGE_NAME "${PARAMS_NAME}")
    set(3RD_PACKAGE_FULL_NAME "${PARAMS_NAME}-${PARAMS_VERSION}")
    set(3RD_PACKAGE_SOURCE_DIR "${3RD_SOURCE_DIR}/${3RD_PACKAGE_FULL_NAME}")
    if (${PARAMS_BUILD})
        set(3RD_PACKAGE_BINARY_DIR "${3RD_BINARY_DIR}/${3RD_PACKAGE_NAME}")
        set(3RD_PACKAGE_INSTALL_DIR "${3RD_INSTALL_DIR}/${3RD_PACKAGE_NAME}/$<CONFIG>")
    endif()

    ExtractZip(
        SRC ${3RD_ZIP_DIR}/${3RD_PACKAGE_FULL_NAME}.zip
        DST ${3RD_SOURCE_DIR}/${3RD_PACKAGE_FULL_NAME}
    )
    if (${PARAMS_BUILD})
        ExternalProject_Add(
            ${3RD_PACKAGE_NAME}
            SOURCE_DIR ${3RD_PACKAGE_SOURCE_DIR}
            BINARY_DIR ${3RD_PACKAGE_BINARY_DIR}
            CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${3RD_PACKAGE_INSTALL_DIR} ${PARAMS_ARG}
            BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config $<CONFIG>
            INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR> --config $<CONFIG>
        )
    endif()

    set(${3RD_PACKAGE_NAME}_SOURCE_DIR ${3RD_PACKAGE_SOURCE_DIR} CACHE PATH "" FORCE)
    if (${PARAMS_BUILD})
        set(${3RD_PACKAGE_NAME}_BINARY_DIR ${3RD_PACKAGE_BINARY_DIR} CACHE PATH "" FORCE)
        set(${3RD_PACKAGE_NAME}_INSTALL_DIR ${3RD_PACKAGE_INSTALL_DIR} CACHE PATH "" FORCE)
    endif()
endfunction()
