function(download_toml)
    message(STATUS "Downloading toml.hpp")

    include(FetchContent)

    set(TOML_URL "https://github.com/ToruNiina/toml11/archive/refs/tags/v4.4.0.tar.gz")
    set(TOML_HASH "SHA256=815bfe6792aa11a13a133b86e7f0f45edc5d71eb78f5fb6686c49c7f792b9049")

    set(possible_file_locations
        ${CMAKE_SOURCE_DIR}/third-part/toml11-4.4.0.tar.gz
    )

    foreach(f IN LISTS possible_file_locations)
        if(EXISTS ${f})
            set(TOML_URL  "${f}")
            file(TO_CMAKE_PATH "${TOML_URL}" TOML_URL)
            message(STATUS "Found local downloaded toml: ${TOML_URL}")
            break()
        endif()
    endforeach()

    FetchContent_Declare(toml
        URL ${TOML_URL}
        URL_HASH    ${TOML_HASH}
    )
    FetchContent_GetProperties(toml)
    if(NOT toml_POPULATED)
        message(STATUS "Downloading toml from ${TOML_URL}")
        FetchContent_Populate(toml)
    endif()
    message(STATUS "toml is downloaded to ${toml_SOURCE_DIR}")

    add_library(toml INTERFACE IMPORTED)
    set_target_properties(toml PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${toml_SOURCE_DIR}/single_include"
    )
        
endfunction(download_toml)


download_toml()