add_executable(decomposition-test ClusterMappingTest.cpp ConfigReaderTest.cpp CSVDataSourceTest.cpp DecompostionTreeTest.cpp
        DoublyLinkedListTest.cpp EdgeTest.cpp GraphTest.cpp ListLinearHeapTest.cpp MergeComponentTest.cpp
        ParmetisIntegrationTest.cpp TreeLeafTest.cpp TreeNodeTest.cpp TreeTest.cpp UnionFindTest.cpp MetisStaticTest.cpp
        IntegerFilterTest.cpp EdgeFilterCoreTest.cpp EdgeFilterCoreHaloTest.cpp ComponentDecompositionTest.cpp)
target_include_directories(decomposition-test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
if (USE_METIS)
    target_link_libraries(decomposition-test parmetis)
endif ()

target_link_libraries(decomposition-test decomposition-alg)
set_target_properties(decomposition-test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/test-input")
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test-input/test_csv_data.csv
        ${CMAKE_CURRENT_SOURCE_DIR}/test-input/testConfig.txt
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test-input)