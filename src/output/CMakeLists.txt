add_library(output CLSWriter.h)

set_target_properties(output PROPERTIES LINKER_LANGUAGE CXX)

include_directories(${CMAKE_SOURCE_DIR}/src/output)