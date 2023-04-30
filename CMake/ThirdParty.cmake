option(CUSTOM_3RD_REPO "using custom 3rd repo" OFF)

if (${CUSTOM_3RD_REPO})
    set(3RD_REPO ${3RD_REPO_URL} CACHE STRING "" FORCE)
else()
    set(3RD_REPO "http://1.13.181.171" CACHE STRING "" FORCE)
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
    cmake_parse_arguments(PARAMS "" "OUTPUT;SOURCE_DIR;BINARY_DIR;INSTALL_DIR" "INPUT" ${ARGN})

    foreach(I ${PARAMS_INPUT})
        set(TEMP "${I}")
        if (DEFINED PARAMS_SOURCE_DIR)
            string(REPLACE "$<SOURCE_DIR>" ${PARAMS_SOURCE_DIR} TEMP ${TEMP})
        endif()
        if (DEFINED PARAMS_BINARY_DIR)
            string(REPLACE "$<BINARY_DIR>" ${PARAMS_BINARY_DIR} TEMP ${TEMP})
        endif()
        if (DEFINED PARAMS_INSTALL_DIR)
            string(REPLACE "$<INSTALL_DIR>" ${PARAMS_INSTALL_DIR} TEMP ${TEMP})
        endif()
        list(APPEND RESULT ${TEMP})
    endforeach()

    set(${PARAMS_OUTPUT} ${RESULT} PARENT_SCOPE)
endfunction()

function(Get3rdPlatformPath)
    cmake_parse_arguments(PARAMS "" "OUTPUT" "INPUT" ${ARGN})

    set(PLATFORM_KEYWORDS "Windows;Darwin;Linux")

    set(HAS_KEYWORDS FALSE)
    foreach (I ${PARAMS_INPUT})
        foreach (K ${PLATFORM_KEYWORDS})
            if (${I} STREQUAL ${K})
                set(HAS_KEYWORDS TRUE)
                break()
            endif()
        endforeach()

        if (${HAS_KEYWORDS})
            break()
        endif()
    endforeach()

    set(START_LOG FALSE)
    foreach (I ${PARAMS_INPUT})
        if ((NOT ${START_LOG}) AND (${I} STREQUAL ${CMAKE_SYSTEM_NAME}))
            set(START_LOG TRUE)
            continue()
        endif()

        if (NOT ${START_LOG})
            continue()
        endif()

        set(END_LOG FALSE)
        foreach (K ${PLATFORM_KEYWORDS})
            if (${K} STREQUAL ${I})
                set(END_LOG TRUE)
                break()
            endif()
        endforeach()

        if (${END_LOG})
            break()
        endif()

        list(APPEND RESULT ${I})
    endforeach ()

    if (${HAS_KEYWORDS})
        set(${PARAMS_OUTPUT} ${RESULT} PARENT_SCOPE)
    else()
        set(${PARAMS_OUTPUT} ${PARAMS_INPUT} PARENT_SCOPE)
    endif()
endfunction()

function(Add3rdHeaderOnlyPackage)
    cmake_parse_arguments(PARAMS "" "NAME;PLATFORM;VERSION;HASH" "INCLUDE" ${ARGN})

    if ((NOT (${PARAMS_PLATFORM} STREQUAL "All")) AND (NOT (${PARAMS_PLATFORM} STREQUAL ${CMAKE_SYSTEM_NAME})))
        return()
    endif()

    set(NAME "${PARAMS_NAME}")
    set(FULL_NAME "${PARAMS_NAME}-${PARAMS_PLATFORM}-${PARAMS_VERSION}")
    set(URL "${3RD_REPO}/${FULL_NAME}.7z")
    set(ZIP "${3RD_ZIP_DIR}/${FULL_NAME}.7z")
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
            3RD_INCLUDE "${R_INCLUDE}"
        )
    endif()
endfunction()

function(Add3rdBinaryPackage)
    cmake_parse_arguments(PARAMS "" "NAME;PLATFORM;VERSION;HASH" "INCLUDE;LINK;LIB;RUNTIME_DEP" ${ARGN})

    if ((NOT (${PARAMS_PLATFORM} STREQUAL "All")) AND (NOT (${PARAMS_PLATFORM} STREQUAL ${CMAKE_SYSTEM_NAME})))
        return()
    endif()

    set(NAME "${PARAMS_NAME}")
    set(FULL_NAME "${PARAMS_NAME}-${PARAMS_PLATFORM}-${PARAMS_VERSION}")
    set(URL "${3RD_REPO}/${FULL_NAME}.7z")
    set(ZIP "${3RD_ZIP_DIR}/${FULL_NAME}.7z")
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
            3RD_INCLUDE "${R_INCLUDE}"
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
            3RD_LINK "${R_LINK}"
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
            3RD_LIB "${R_LIB}"
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
            3RD_RUNTIME_DEP "${P_RUNTIME_DEP}"
        )
    endif()
endfunction()

function(Add3rdCMakeProject)
    cmake_parse_arguments(PARAMS "" "NAME;PLATFORM;VERSION;HASH" "CMAKE_ARG;INCLUDE;LINK;LIB;RUNTIME_DEP" ${ARGN})

    if ((NOT (${PARAMS_PLATFORM} STREQUAL "All")) AND (NOT (${PARAMS_PLATFORM} STREQUAL ${CMAKE_SYSTEM_NAME})))
        return()
    endif()

    set(NAME "${PARAMS_NAME}")
    set(FULL_NAME "${PARAMS_NAME}-${PARAMS_PLATFORM}-${PARAMS_VERSION}")
    set(URL "${3RD_REPO}/${FULL_NAME}.7z")
    set(ZIP "${3RD_ZIP_DIR}/${FULL_NAME}.7z")
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
        Get3rdPlatformPath(
            INPUT ${R_INCLUDE}
            OUTPUT P_INCLUDE
        )
        set_target_properties(
            ${NAME} PROPERTIES
            3RD_INCLUDE "${P_INCLUDE}"
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
        Get3rdPlatformPath(
            INPUT ${R_LINK}
            OUTPUT P_LINK
        )
        set_target_properties(
            ${NAME} PROPERTIES
            3RD_LINK "${P_LINK}"
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
        Get3rdPlatformPath(
            INPUT ${R_LIB}
            OUTPUT P_LIB
        )
        set_target_properties(
            ${NAME} PROPERTIES
            3RD_LIB "${P_LIB}"
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
        Get3rdPlatformPath(
            INPUT ${R_RUNTIME_DEP}
            OUTPUT P_RUNTIME_DEP
        )
        set_target_properties(
            ${NAME} PROPERTIES
            3RD_RUNTIME_DEP "${P_RUNTIME_DEP}"
        )
    endif()
endfunction()

function(Add3rdAliasPackage)
    cmake_parse_arguments(PARAMS "" "NAME;PLATFORM" "LIB" ${ARGN})

    if ((NOT (${PARAMS_PLATFORM} STREQUAL "All")) AND (NOT (${PARAMS_PLATFORM} STREQUAL ${CMAKE_SYSTEM_NAME})))
        return()
    endif()

    add_custom_target(${PARAMS_NAME} ALL)
    set_target_properties(
        ${PARAMS_NAME} PROPERTIES
        3RD_TYPE "Alias"
        3RD_LIB "${PARAMS_LIB}"
    )
endfunction()
