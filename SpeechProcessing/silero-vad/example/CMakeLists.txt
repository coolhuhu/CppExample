add_executable(
    silero-vad-example
    silero-vad-example.cc
)

if(NOT DEFINED onnxruntime_lib_files)
    target_link_libraries(
        silero-vad-example
        PUBLIC
        onnxruntime
    )
else()
    target_link_libraries(
        silero-vad-example 
        PUBLIC
        ${onnxruntime_lib_files}
    )
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Darwin" AND CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
    target_link_libraries(
        silero-vad-example 
        PUBLIC
        "-framework Foundation"
        "-framework CoreFoundation"
    )
endif()
