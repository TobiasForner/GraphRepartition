CMAKE_MINIMUM_REQUIRED(VERSION 3.15)
project(dbgtthesis CXX)

set(CMAKE_CXX_STANDARD 14)

option(BUILD_TESTS "specifies whether to build tests. Requires catch.h to be inserted into the test directory or for it to be reconfigured by hand" OFF)

option(WITH_DEBUG "perform debug checks" OFF)

option(GATHER_STATS "gather run statistics and print the periodically" OFF)

#options for specifying which elements are used

option(USE_LOGGING "specify whether to use logging. Needs spdlog to be in the source directory" OFF)

if (USE_LOGGING)
    message("logging enabled")
    add_definitions(-DLOGGING_ENABLED)
endif ()

#option(USE_MONGODB "specify whether to use mongodb. Needs mongodb dependencies" OFF)

#if (USE_MONGODB)
#    add_definitions(-DMONGODB_ENABLED)
#endif ()

option(USE_METIS "specify whether to use METIS or ParMETIS. Requires ParMETIS to be inserted into the lib directory" OFF)

if (USE_METIS)
    add_definitions(-DMETIS_ENABLED)
endif ()

#add definitions for other cmake options
if (WITH_DEBUG)
    message("with debug")
    add_definitions(-DUSE_DEBUG)

    #used for profiling/debugging
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
endif ()

if (FORCE_PROFILING)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
endif ()

if (GATHER_STATS)
    message("stat collection and printing enabled")
    add_definitions(-DCOLLECT_STATS)
endif ()

add_subdirectory(src)
if (BUILD_TESTS)
    add_subdirectory(tests)
endif ()