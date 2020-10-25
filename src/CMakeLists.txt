#look for required packages

find_package(libmongocxx REQUIRED)
find_package(libbsoncxx REQUIRED)
if (USE_METIS)
    find_package(MPI REQUIRED)
endif ()

#add required libraries
add_library(libmongocxx INTERFACE IMPORTED)
add_library(libbsoncxx INTERFACE IMPORTED)
if (USE_METIS)
    add_library(MPI INTERFACE IMPORTED)
endif ()

set_property(TARGET libmongocxx PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES ${LIBMONGOCXX_INCLUDE_DIRS})
set_property(TARGET libbsoncxx PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES ${LIBBSONCXX_INCLUDE_DIRS})
if (USE_METIS)
    set_property(TARGET MPI PROPERTY
            INTERFACE_INCLUDE_DIRECTORIES ${MPI_INCLUDE_DIRS})
endif ()


add_subdirectory(basic-profiling)
add_subdirectory(output)
if (USE_LOGGING)
    add_subdirectory(spdlog)
endif ()
if (USE_METIS)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../lib/ParMETIS-4.0.3 ${CMAKE_CURRENT_BINARY_DIR}/parmetis_build)
endif ()


if (USE_LOGGING)
    if (NOT TARGET spdlog)
        # Stand-alone build
        find_package(spdlog REQUIRED)
    endif ()
endif ()

add_library(decomposition-alg data_structures/Tree.h DecompositionTree.cpp DecompositionTree.h TreeNode.h
        TreeLeaf.h data_structures/ListLinearHeap.h data_structures/DoublyLinkedList.h data_structures/DoublyLinkedListNode.h data_structures/ListLinearHeapElement.h
        data_structures/Graph.cpp data_structures/Graph.h Edge.h typedefs.h data_sources/DataSource.h data_sources/MongoDBData.cpp data_sources/MongoDBData.h
        ParMetisConnectedComponentComputation.cpp ConfigReader.cpp ConfigReader.h DecompositionMethod.h
        data_sources/MongoDBDataExperimental.cpp data_sources/MongoDBDataExperimental.h data_structures/UnionFind.h ClusterMapping.cpp ClusterMapping.h
        check_cluster_mapping.h data_structures/DoublyLinkedListIterator.h data_sources/CSVDataSource.cpp
        data_sources/CSVDataSource.h Filter.h EdgeFilterCore.h dynamicLoop.h staticLoop.h MetisStatic.h IntegerFilter.h
        EdgeFilterCoreHalo.h ComponentDecomposition.cpp)

if (USE_LOGGING)
    target_include_directories(decomposition-alg PUBLIC spdlog/include)
    target_link_libraries(decomposition-alg spdlog)
endif ()

if (USE_METIS)
    target_include_directories(decomposition-alg PUBLIC ../lib/ParMETIS-4.0.3 ${MPI_INCLUDE_DIRS})
    target_link_libraries(decomposition-alg ${MPI_LIBRARIES} parmetis)
endif ()

target_include_directories(decomposition-alg PUBLIC ${LIBMONGOCXX_INCLUDE_DIRS} ${LIBBSONCXX_INCLUDE_DIRS})
target_link_libraries(decomposition-alg ${LIBMONGOCXX_LIBRARIES} ${lIBBSONCXX_LIBRARIES} output basic-profiling)

if (USE_METIS)
    set_target_properties(parmetis PROPERTIES COMPILE_FLAGS "-w")
    target_compile_options(parmetis PRIVATE "-Wno-misleading-indentation")

    if (MPI_COMPILE_FLAGS)
        set_target_properties(decomposition-alg PROPERTIES
                COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
    endif ()

    if (MPI_LINK_FLAGS)
        set_target_properties(decomposition-alg PROPERTIES
                LINK_FLAGS "${MPI_LINK_FLAGS}")
    endif ()
endif ()

#target_compile_options(decomposition-alg PRIVATE -Werror -Wall -Wextra)

if (USE_LOGGING)
    spdlog_enable_warnings(decomposition-alg)
endif ()


add_executable(cluster-alg main.cpp)
target_link_libraries(cluster-alg decomposition-alg)
set_target_properties(cluster-alg PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        )


file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/logs")