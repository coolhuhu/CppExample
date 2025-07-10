

add_executable(
    onnxruntime-tensor-test
    onnxruntime-tensor-test.cc
    ${PROJECT_SOURCE_DIR}/onnxruntime-utils/onnxruntime-tensor.cc
)
target_include_directories(
    onnxruntime-tensor-test
    PUBLIC
    ${PROJECT_SOURCE_DIR}/onnxruntime-utils
)
if(NOT DEFINED onnxruntime_lib_files)
    target_link_libraries(
        onnxruntime-tensor-test 
        PUBLIC
        onnxruntime
    )
else()
    target_link_libraries(
        onnxruntime-tensor-test 
        PUBLIC
        ${onnxruntime_lib_files}
    )
endif()

