function(download_json)
    message(STATUS "Downloading json.hpp")

    include(FetchContent)

    set(JSON_URL "https://github.com/nlohmann/json/releases/download/v3.12.0/include.zip")
    set(JSON_HASH "SHA256=b8cb0ef2dd7f57f18933997c9934bb1fa962594f701cd5a8d3c2c80541559372")

    set(possible_file_locations
        ${CMAKE_SOURCE_DIR}/third-part/json.zip
    )

    foreach(f IN LISTS possible_file_locations)
        if(EXISTS ${f})
            set(JSON_URL  "${f}")
            file(TO_CMAKE_PATH "${JSON_URL}" JSON_URL)
            message(STATUS "Found local downloaded json: ${JSON_URL}")
            break()
        endif()
    endforeach()

    FetchContent_Declare(json
        URL ${JSON_URL}
        URL_HASH    ${JSON_HASH}
        SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/_deps/json-src"
    )
    FetchContent_GetProperties(json)
    if(NOT json_POPULATED)
        message(STATUS "Downloading json from ${JSON_URL}")
        FetchContent_Populate(json)
    endif()
    message(STATUS "json is downloaded to ${json_SOURCE_DIR}")

    add_library(json INTERFACE IMPORTED)
    set_target_properties(json PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${json_SOURCE_DIR}/include"
    )
        
endfunction(download_json)


download_json()