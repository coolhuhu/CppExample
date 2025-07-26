add_executable(example example.cc)
target_link_libraries(
    example PUBLIC
    portaudio_static
)

add_executable(paex_saw paex_saw.cc)
target_link_libraries(
    paex_saw PUBLIC
    portaudio_static
)

add_executable(paex_record paex_record.cc)
target_link_libraries(
    paex_record PUBLIC
    portaudio_static
)

add_executable(record-to-file record-to-file.cc)
target_link_libraries(
    record-to-file PUBLIC
    portaudio_static
)

add_executable(play-wav play-wav.cc)
target_link_libraries(
    play-wav PUBLIC
    portaudio_static
)