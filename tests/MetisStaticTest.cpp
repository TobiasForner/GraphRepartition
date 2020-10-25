//
// Created by tobias on 15.02.20.
//
#ifdef METIS_ENABLED
#include "catch.h"
#include "MetisStatic.h"
#include "check_cluster_mapping.h"

TEST_CASE("testMetisStatic", "[MetisStaticTest]") {
    auto g = decomposition_tree::Graph(4);
    auto e1 = decomposition_tree::Edge(0, 1, 1);
    auto e2 = decomposition_tree::Edge(1, 2, 2);
    auto e3 = decomposition_tree::Edge(2, 3, 1);
    auto e4 = decomposition_tree::Edge(3, 1, 1);
    auto e5 = decomposition_tree::Edge(0, 1, 1);
    g.addEdge(e1);
    g.addEdge(e2);
    g.addEdge(e3);
    g.addEdge(e4);
    g.addEdge(e5);

    auto initialMapping = std::vector<unsigned int>();
    initialMapping.emplace_back(0);
    initialMapping.emplace_back(0);
    initialMapping.emplace_back(1);
    initialMapping.emplace_back(1);
    auto newMapping = std::vector<unsigned int>();
    unsigned int timePassed = 0;

    run_METIS_static(g, 2, initialMapping, newMapping, timePassed);

    REQUIRE(decomposition_tree::check_mapping(newMapping, 2, 2.0));
}

#endif