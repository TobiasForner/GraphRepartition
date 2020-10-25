//
// Created by tobias on 29.02.20.
//

#include "EdgeFilterCoreHalo.h"
#include "catch.h"

TEST_CASE("EdgeFilterCoreHaloTest", "[EdgeFilterCoreHaloTest]") {
    std::vector<unsigned int> nodesToPos{3, 2, 4, 1, 5, 0, 7, 6, 8, 9};
    auto filter = EdgeFilterCoreHalo(3, 7, nodesToPos);
    auto edgeAccept = decomposition_tree::Edge(0, 2, 1);
    auto edgeNotFiltered = decomposition_tree::Edge(8, 9, 6);
    auto edgeFiltered2 = decomposition_tree::Edge(0, 9, 6);
    auto edgeFiltered3 = decomposition_tree::Edge(3, 2, 6);
    CHECK(filter(&edgeAccept));
    CHECK_FALSE(filter(&edgeNotFiltered));
    CHECK(filter(&edgeFiltered2));
    CHECK(filter(&edgeFiltered3));
}

