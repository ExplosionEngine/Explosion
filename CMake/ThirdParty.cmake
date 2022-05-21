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

function(DownloadAndExtract3rdPackage)
    cmake_parse_arguments(PARAMS "" "URL;SAVE_AS;EXTRACT_TO;HASH" "ARG" ${ARGN})

    if (EXISTS ${PARAMS_SAVE_AS})
        message("found downloaded file for ${PARAMS_URL} -> ${PARAMS_SAVE_AS}")
    else()
        message("starting download package ${PARAMS_URL}")
        file(
            DOWNLOAD
            ${PARAMS_URL} ${PARAMS_SAVE_AS}
            SHOW_PROGRESS
        )
    endif()

    if (DEFINED PARAMS_HASH)
        file(SHA256 ${PARAMS_SAVE_AS} HASH_VALUE)
        if (NOT (${PARAMS_HASH} STREQUAL ${HASH_VALUE}))
            message(FATAL_ERROR "check hash value failed for file ${PARAMS_SAVE_AS}, given ${PARAMS_HASH} actual ${HASH_VALUE}")
        endif ()
    endif()

    if (NOT EXISTS ${PARAMS_EXTRACT_TO})
        file(
            ARCHIVE_EXTRACT
            INPUT ${PARAMS_SAVE_AS}
            DESTINATION ${PARAMS_EXTRACT_TO}
        )
    endif()
endfunction()

function(Expand3rdPathExpression)
    cmake_parse_arguments(PARAMS "" "INPUT;OUTPUT;SOURCE_DIR;BINARY_DIR;INSTALL_DIR" "" ${ARGN})

    set(TEMP "${PARAMS_INPUT}")
    if (DEFINED PARAMS_SOURCE_DIR)
        string(REPLACE "$<SOURCE_DIR>" ${PARAMS_SOURCE_DIR} TEMP ${TEMP})
    endif()
    if (DEFINED PARAMS_BINARY_DIR)
        string(REPLACE "$<BINARY_DIR>" ${PARAMS_BINARY_DIR} TEMP ${TEMP})
    endif()
    if (DEFINED PARAMS_INSTALL_DIR)
        string(REPLACE "$<INSTALL_DIR>" ${PARAMS_INSTALL_DIR} TEMP ${TEMP})
    endif()

    set(${PARAMS_OUTPUT} ${TEMP} PARENT_SCOPE)
endfunction()

function(Add3rdHeaderOnlyPackage)
    cmake_parse_arguments(PARAMS "" "NAME;PLATFORM;VERSION;HASH" "INCLUDE" ${ARGN})

    set(NAME "${PARAMS_NAME}")
    set(FULL_NAME "${PARAMS_NAME}-${PARAMS_PLATFORM}-${PARAMS_VERSION}")
    set(URL "${3RD_REPO}/${FULL_NAME}.zip")
    set(ZIP "${3RD_ZIP_DIR}/${FULL_NAME}.zip")
    set(SOURCE_DIR "${3RD_SOURCE_DIR}/${FULL_NAME}")

    DownloadAndExtract3rdPackage(
        URL ${URL}
        SAVE_AS ${ZIP}
        EXTRACT_TO ${SOURCE_DIR}
        HASH ${PARAMS_HASH}
    )

    add_custom_target(${NAME} ALL)
    set_target_properties(
        ${NAME} PROPERTIES
        3RD_TYPE "HeaderOnly"
    )

    if (DEFINED PARAMS_INCLUDE)
        Expand3rdPathExpression(
            INPUT ${PARAMS_INCLUDE}
            OUTPUT R_INCLUDE
            SOURCE_DIR ${SOURCE_DIR}
        )
        set_target_properties(
            ${NAME} PROPERTIES
            INCLUDE ${R_INCLUDE}
        )
    endif()
endfunction()

function(Add3rdBinaryPackage)
    cmake_parse_arguments(PARAMS "" "NAME;PLATFORM;VERSION;HASH" "INCLUDE;LINK;LIB;RUNTIME_DEP" ${ARGN})

    set(NAME "${PARAMS_NAME}")
    set(FULL_NAME "${PARAMS_NAME}-${PARAMS_PLATFORM}-${PARAMS_VERSION}")
    set(URL "${3RD_REPO}/${FULL_NAME}.zip")
    set(ZIP "${3RD_ZIP_DIR}/${FULL_NAME}.zip")
    set(SOURCE_DIR "${3RD_SOURCE_DIR}/${FULL_NAME}")

    DownloadAndExtract3rdPackage(
        URL ${URL}
        SAVE_AS ${ZIP}
        EXTRACT_TO ${SOURCE_DIR}
        HASH ${PARAMS_HASH}
    )

    add_custom_target(${NAME} ALL)
    set_target_properties(
        ${NAME} PROPERTIES
        3RD_TYPE "Binary"
    )

    if (DEFINED PARAMS_INCLUDE)
        Expand3rdPathExpression(
            INPUT ${PARAMS_INCLUDE}
            OUTPUT R_INCLUDE
            SOURCE_DIR ${SOURCE_DIR}
        )
        set_target_properties(
            ${NAME} PROPERTIES
            INCLUDE ${R_INCLUDE}
        )
    endif()

    if (DEFINED PARAMS_LINK)
        Expand3rdPathExpression(
            INPUT ${PARAMS_LINK}
            OUTPUT R_LINK
            SOURCE_DIR ${SOURCE_DIR}
        )
        set_target_properties(
            ${NAME} PROPERTIES
            LINK ${R_LINK}
        )
    endif()

    if (DEFINED PARAMS_LIB)
        Expand3rdPathExpression(
            INPUT ${PARAMS_LIB}
            OUTPUT R_LIB
            SOURCE_DIR ${SOURCE_DIR}
        )
        set_target_properties(
            ${NAME} PROPERTIES
            LIB ${R_LIB}
        )
    endif()

    if (DEFINED PARAMS_RUNTIME_DEP)
        Expand3rdPathExpression(
            INPUT ${PARAMS_RUNTIME_DEP}
            OUTPUT R_RUNTIME_DEP
            SOURCE_DIR ${SOURCE_DIR}
        )
        set_target_properties(
            ${NAME} PROPERTIES
            RUNTIME_DEP ${R_RUNTIME_DEP}
        )
    endif()
endfunction()

function(Add3rdCMakeProject)
    cmake_parse_arguments(PARAMS "" "NAME;PLATFORM;VERSION;HASH" "CMAKE_ARG;INCLUDE;LINK;LIB;RUNTIME_DEP" ${ARGN})

    set(NAME "${PARAMS_NAME}")
    set(FULL_NAME "${PARAMS_NAME}-${PARAMS_PLATFORM}-${PARAMS_VERSION}")
    set(URL "${3RD_REPO}/${FULL_NAME}.zip")
    set(ZIP "${3RD_ZIP_DIR}/${FULL_NAME}.zip")
    set(SOURCE_DIR "${3RD_SOURCE_DIR}/${FULL_NAME}")
    set(BINARY_DIR "${3RD_BINARY_DIR}/${NAME}")
    set(INSTALL_DIR "${3RD_INSTALL_DIR}/${NAME}/$<CONFIG>")

    DownloadAndExtract3rdPackage(
        URL ${URL}
        SAVE_AS ${ZIP}
        EXTRACT_TO ${SOURCE_DIR}
        HASH ${PARAMS_HASH}
    )

    ExternalProject_Add(
        ${NAME}
        SOURCE_DIR ${SOURCE_DIR}
        BINARY_DIR ${BINARY_DIR}
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} ${PARAMS_CMAKE_ARG}
        BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config $<CONFIG>
        INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR> --config $<CONFIG>
    )
    set_target_properties(
        ${NAME} PROPERTIES
        3RD_TYPE "CMakeProject"
    )

    if (DEFINED PARAMS_INCLUDE)
        Expand3rdPathExpression(
            INPUT ${PARAMS_INCLUDE}
            OUTPUT R_INCLUDE
            SOURCE_DIR ${SOURCE_DIR}
            BINARY_DIR ${BINARY_DIR}
            INSTALL_DIR ${INSTALL_DIR}
        )
        set_target_properties(
            ${NAME} PROPERTIES
            INCLUDE ${R_INCLUDE}
        )
    endif()

    if (DEFINED PARAMS_LINK)
        Expand3rdPathExpression(
            INPUT ${PARAMS_LINK}
            OUTPUT R_LINK
            SOURCE_DIR ${SOURCE_DIR}
            BINARY_DIR ${BINARY_DIR}
            INSTALL_DIR ${INSTALL_DIR}
        )
        set_target_properties(
            ${NAME} PROPERTIES
            LINK ${R_LINK}
        )
    endif()

    if (DEFINED PARAMS_LIB)
        Expand3rdPathExpression(
            INPUT ${PARAMS_LIB}
            OUTPUT R_LIB
            SOURCE_DIR ${SOURCE_DIR}
            BINARY_DIR ${BINARY_DIR}
            INSTALL_DIR ${INSTALL_DIR}
        )
        set_target_properties(
            ${NAME} PROPERTIES
            LIB ${R_LIB}
        )
    endif()

    if (DEFINED PARAMS_RUNTIME_DEP)
        Expand3rdPathExpression(
            INPUT ${PARAMS_RUNTIME_DEP}
            OUTPUT R_RUNTIME_DEP
            SOURCE_DIR ${SOURCE_DIR}
            BINARY_DIR ${BINARY_DIR}
            INSTALL_DIR ${INSTALL_DIR}
        )
        set_target_properties(
            ${NAME} PROPERTIES
            RUNTIME_DEP ${R_RUNTIME_DEP}
        )
    endif()
endfunction()

function(Add3rdAliasPackage)
    cmake_parse_arguments(PARAMS "" "NAME;PLATFORM" "LIB" ${ARGN})

    add_custom_target(${PARAMS_NAME} ALL)
    set_target_properties(
        ${PARAMS_NAME} PROPERTIES
        3RD_TYPE "Alias"
        LIB ${PARAMS_LIB}
    )
endfunction()
