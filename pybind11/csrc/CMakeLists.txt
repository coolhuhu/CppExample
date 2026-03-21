include_directories(${CMAKE_SOURCE_DIR})

add_library(pymodule SHARED pymath.cc)
set_target_properties(
  pymodule
  PROPERTIES
  POSITION_INDEPENDENT_CODE ON
)
install(TARGETS pymodule DESTINATION lib)
install(FILES pymath.h DESTINATION include)