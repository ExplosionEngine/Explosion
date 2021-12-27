set(THIRD_PARTY_PATH "${CMAKE_SOURCE_DIR}/ThirdParty")
set(THIRD_PARTY_ZIP_PATH "${THIRD_PARTY_PATH}/Zip")
set(THIRD_PARTY_LIB_PATH "${THIRD_PARTY_PATH}/Lib")

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
function(AddThirdPartyPackage)
    cmake_parse_arguments(PARAMS "" "NAME;VERSION" "ARG" ${ARGN})

    set(3RD_PACKAGE_FULL_NAME "${PARAMS_NAME}-${PARAMS_VERSION}")
    set(3RD_PACKAGE_SOURCE_DIR "${THIRD_PARTY_LIB_PATH}/${3RD_PACKAGE_FULL_NAME}")
    set(3RD_PACKAGE_BINARY_DIR "${CMAKE_BINARY_DIR}/ThirdPartyBuild/${3RD_PACKAGE_FULL_NAME}")
    set(3RD_PACKAGE_INSTALL_DIR "${CMAKE_BINARY_DIR}/ThirdPartyInstall/${3RD_PACKAGE_FULL_NAME}")

    ExtractZip(
        SRC ${THIRD_PARTY_ZIP_PATH}/${3RD_PACKAGE_FULL_NAME}.zip
        DST ${THIRD_PARTY_LIB_PATH}/${3RD_PACKAGE_FULL_NAME}
    )
    ExternalProject_Add(
        ${3RD_PACKAGE_FULL_NAME}
        SOURCE_DIR ${3RD_PACKAGE_SOURCE_DIR}
        BINARY_DIR ${3RD_PACKAGE_BINARY_DIR}
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${3RD_PACKAGE_INSTALL_DIR}/$<CONFIG> ${PARAMS_ARG}
        BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config $<CONFIG>
        INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR> --config $<CONFIG>
    )
endfunction()
