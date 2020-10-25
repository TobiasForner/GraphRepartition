//
// Created by tobias on 29.02.20.
//

#include "EdgeFilterCore.h"
#include "catch.h"

TEST_CASE("EdgeFilterCoreTest", "[EdgeFilterCoreTest]") {
    std::vector<unsigned int> nodesToPos{3, 2, 4, 1, 5, 0, 7, 6, 8, 9};
    auto filter = EdgeFilterCore(3, 7, nodesToPos);
    auto edgeAccept = decomposition_tree::Edge(0, 2, 1);
    auto edgeNotFiltered = decomposition_tree::Edge(8, 9, 6);
    auto edgeNotFiltered2 = decomposition_tree::Edge(0, 9, 6);
    auto edgeNotFiltered3 = decomposition_tree::Edge(3, 2, 6);
    CHECK(filter(&edgeAccept));
    CHECK_FALSE(filter(&edgeNotFiltered));
    CHECK_FALSE(filter(&edgeNotFiltered2));
    CHECK_FALSE(filter(&edgeNotFiltered3));
}

