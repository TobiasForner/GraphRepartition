add_library(basic-profiling IterationTimeWriter.cpp)
target_include_directories(basic-profiling PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../output>)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../basic-profiling)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../basic-profiling)
target_link_libraries(basic-profiling PUBLIC output)