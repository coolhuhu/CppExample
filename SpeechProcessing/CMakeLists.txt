cmake_minimum_required(VERSION 3.16)
project(SpeechProcessing)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(onnxruntime)

add_subdirectory(silero-vad)
