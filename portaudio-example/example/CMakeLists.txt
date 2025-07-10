add_executable(example example.cc)
target_link_libraries(
    example PUBLIC
    portaudio_static
)