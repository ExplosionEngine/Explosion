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
            message(FATAL_ERROR "check hash value failed for file ${PARAMS_SAVE_AS}")
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

function(AddThirdPartyPackage)
    cmake_parse_arguments(PARAMS "BUILD" "NAME;VERSION;HASH" "ARG" ${ARGN})

    set(NAME "${PARAMS_NAME}")
    set(FULL_NAME "${PARAMS_NAME}-${PARAMS_VERSION}")
    set(URL "${3RD_REPO}/${FULL_NAME}.zip")
    set(ZIP "${3RD_ZIP_DIR}/${FULL_NAME}.zip")
    set(SOURCE_DIR "${3RD_SOURCE_DIR}/${FULL_NAME}")

    if (${PARAMS_BUILD})
        set(BINARY_DIR "${3RD_BINARY_DIR}/${NAME}")
        set(INSTALL_DIR "${3RD_INSTALL_DIR}/${NAME}/$<CONFIG>")
    endif()

    DownloadAndExtract3rdPackage(
        URL ${URL}
        SAVE_AS ${ZIP}
        EXTRACT_TO ${SOURCE_DIR}
        HASH ${PARAMS_HASH}
    )

    if (${PARAMS_BUILD})
        ExternalProject_Add(
            ${NAME}
            SOURCE_DIR ${SOURCE_DIR}
            BINARY_DIR ${BINARY_DIR}
            CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} ${PARAMS_ARG}
            BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config $<CONFIG>
            INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR> --config $<CONFIG>
        )
    endif()

    set(${NAME}_SOURCE_DIR ${SOURCE_DIR} CACHE PATH "" FORCE)
    if (${PARAMS_BUILD})
        set(${NAME}_BINARY_DIR ${BINARY_DIR} CACHE PATH "" FORCE)
        set(${NAME}_INSTALL_DIR ${INSTALL_DIR} CACHE PATH "" FORCE)
    endif()
endfunction()
