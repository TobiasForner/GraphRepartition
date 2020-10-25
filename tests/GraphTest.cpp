//
// Created by tobias on 01.12.19.
//
#include "catch.h"
#include "../src/data_structures/Graph.h"

TEST_CASE("GraphTest Constructor and Getters", "[GraphTest]") {
    decomposition_tree::Graph g = decomposition_tree::Graph(2);
    REQUIRE(g.getNodesNr() == 2);
    REQUIRE(g.getEdgesNr() == 0);
    REQUIRE(g.getEdges(0).empty());
    REQUIRE(g.getEdges(1).empty());
}

TEST_CASE("Graph addNodeTest", "[GraphTest]") {
    decomposition_tree::Graph g = decomposition_tree::Graph(2);
    g.addNode(3);
    REQUIRE(g.getNodesNr() == 4);
    g.addNode(10);
    REQUIRE(g.getNodesNr() == 11);
    for (decomposition_tree::node_ i = 0; i <= 10; i++) {
        REQUIRE(g.getEdges(i).empty());
    }
    REQUIRE_THROWS(g.getEdges(11));
}

TEST_CASE("Graph addEdgeTest", "[GraphTest]") {
    decomposition_tree::Graph g = decomposition_tree::Graph(2);
    auto e1 = decomposition_tree::Edge(0, 1, 2);
    g.addEdge(e1);
    CHECK(g.getEdgesNr() == 2);
    CHECK(g.getEdges(0).size() == 1);
    CHECK(g.getEdges(1).size() == 1);
    auto e2 = decomposition_tree::Edge(1, 5, 1);
    g.addEdge(e2);
    CHECK(g.getEdgesNr() == 4);
    CHECK(g.getEdges(0).size() == 1);
    CHECK(g.getEdges(1).size() == 2);
    CHECK(g.getEdges(5).size() == 1);
    CHECK(g.getNodesNr() == 6);
    auto e3 = decomposition_tree::Edge(1, 0, 3);
    g.addEdge(e3);
    for(auto e:g.getEdges(1)){
        if(e->getEnd()==0){
            CHECK(e->getWeight()==5);
        }
    }
    for(auto e:g.getEdges(0)){
        if(e->getEnd()==1){
            CHECK(e->getWeight()==5);
        }
    }
}