//
// Created by tobias on 03.11.19.
//

#include "catch.h"
#include "../src/MergeComponent.h"

TEST_CASE("MergeComponent ConstructorTest", "[MergeComponent]") {
    auto toTest = decomposition_tree::MergeComponent(0, 1);
    REQUIRE(toTest.getNodes().size() == 1);
    REQUIRE(toTest.getEdges().empty());
    REQUIRE(toTest.getNumber() == 0);
    REQUIRE(toTest.getDegree() == 0);

    toTest = decomposition_tree::MergeComponent(5, 4);
    REQUIRE(toTest.getNodes().size() == 1);
    REQUIRE(toTest.getEdges().empty());
    REQUIRE(toTest.getNumber() == 5);
    REQUIRE(toTest.getDegree() == 0);
}

TEST_CASE("MergeComponent addNodeTest", "[MergeComponent]") {
    auto toTest = decomposition_tree::MergeComponent(0, 9);
    REQUIRE(toTest.getNodes().size() == 1);
    REQUIRE(toTest.getEdges().empty());
    REQUIRE(toTest.getNumber() == 0);
    REQUIRE(toTest.getDegree() == 0);

    toTest.addNode(2);
    REQUIRE(toTest.getNodes().size() == 2);
    REQUIRE(toTest.getEdges().empty());
    REQUIRE(toTest.getNumber() == 0);
    REQUIRE(toTest.getDegree() == 0);

    toTest.addNode(2);
    REQUIRE(toTest.getNodes().size() == 2);
    REQUIRE(toTest.getEdges().empty());
    REQUIRE(toTest.getNumber() == 0);
    REQUIRE(toTest.getDegree() == 0);

    toTest.addNode(100);
    REQUIRE(toTest.getNodes().size() == 3);
    REQUIRE(toTest.getEdges().empty());
    REQUIRE(toTest.getNumber() == 0);
    REQUIRE(toTest.getDegree() == 0);
}

TEST_CASE("MergeComponent addEdgeTest", "[MergeComponent]") {
    auto toTest = decomposition_tree::MergeComponent(5, 7);
    REQUIRE(toTest.getNodes().size() == 1);
    REQUIRE(toTest.getEdges().empty());
    REQUIRE(toTest.getNumber() == 5);
    REQUIRE(toTest.getDegree() == 0);

    toTest.addEdge(5, 10);
    REQUIRE(toTest.getNodes().size() == 1);
    REQUIRE(toTest.getEdges().empty());
    REQUIRE(toTest.getNumber() == 5);
    REQUIRE(toTest.getDegree() == 0);

    toTest.addEdge(0, 10);
    REQUIRE(toTest.getEdges().size() == 1);
    REQUIRE(toTest.getDegree() == 10);
    REQUIRE(toTest.getNodes().size() == 1);
    toTest.addEdge(0, 10);
    REQUIRE(toTest.getEdges().size() == 1);
    REQUIRE(toTest.getDegree() == 10);
    REQUIRE(toTest.getNodes().size() == 1);
    auto e = new decomposition_tree::Edge(5, 7, 6);
    toTest.addEdge(e);
    REQUIRE(toTest.getEdges().size() == 2);
    REQUIRE(toTest.getDegree() == 16);
    REQUIRE(toTest.getNodes().size() == 1);
}

TEST_CASE("MergeComponent addEdgeNoChecksTest", "[MergeComponent]") {
    decomposition_tree::MergeComponent toTest = decomposition_tree::MergeComponent(5, 7);
    toTest.addEdge(5, 10);
    toTest.addEdge(0, 10);
    toTest.addEdge(0, 10);
    REQUIRE(toTest.getEdges().size() == 1);
    REQUIRE(toTest.getDegree() == 10);
    REQUIRE(toTest.getNodes().size() == 1);
    auto e = new decomposition_tree::Edge(5, 10, 3);
    toTest.addEdgeNoChecks(e);
    REQUIRE(toTest.getEdges().size() == 2);
    REQUIRE(toTest.getDegree() == 13);
    auto e2 = new decomposition_tree::Edge(7, 10, 7);
    toTest.addEdgeNoChecks(e2);
    REQUIRE(toTest.getEdges().size() == 3);
    REQUIRE(toTest.getDegree() == 20);
}

TEST_CASE("MergeComponent mergeEdgeTest", "[MergeComponent]") {
    auto toTest = decomposition_tree::MergeComponent(5, 9);
    REQUIRE(toTest.getNodes().size() == 1);
    REQUIRE(toTest.getEdges().empty());
    REQUIRE(toTest.getNumber() == 5);
    REQUIRE(toTest.getDegree() == 0);

    auto e = new decomposition_tree::Edge(5, 0, 10);
    auto rev = new decomposition_tree::Edge(0, 5, 10);
    e->setReverse(rev);
    rev->setReverse(e);
    toTest.mergeEdge(e);
    REQUIRE(toTest.getEdges().size() == 1);
    REQUIRE(toTest.getDegree() == 10);
    REQUIRE(toTest.getNodes().size() == 1);

    auto e2 = new decomposition_tree::Edge(5, 0, 10);
    toTest.mergeEdge(e2);
    REQUIRE(toTest.getEdges().size() == 1);
    REQUIRE(toTest.getDegree() == 20);
    REQUIRE(toTest.getNodes().size() == 1);

    e = new decomposition_tree::Edge(5, 2, 1);
    rev = new decomposition_tree::Edge(2, 5, 1);
    e->setReverse(rev);
    rev->setReverse(e);
    toTest.mergeEdge(e);
    REQUIRE(toTest.getEdges().size() == 2);
    REQUIRE(toTest.getDegree() == 21);
    REQUIRE(toTest.getNodes().size() == 1);
}

TEST_CASE("MergeComponent delEdgeTest", "[MergeComponent]") {

    auto toTest = decomposition_tree::MergeComponent(5, 5);
    toTest.addEdge(0, 10);
    toTest.addEdge(2, 3);
    toTest.delEdge(0);
    REQUIRE(toTest.getEdges().size() == 1);
    REQUIRE(toTest.getDegree() == 3);

    toTest.delEdge(3);
    REQUIRE(toTest.getEdges().size() == 1);
    REQUIRE(toTest.getDegree() == 3);

    toTest.delEdge(2);
    REQUIRE(toTest.getEdges().empty());
    REQUIRE(toTest.getDegree() == 0);
}

TEST_CASE("MergeComponent delAllEdgesTest", "[MergeComponent]") {
    auto toTest = decomposition_tree::MergeComponent(5, 5);
    toTest.delAllEdges();
    REQUIRE(toTest.getEdges().empty());
    REQUIRE(toTest.getDegree() == 0);

    toTest.addEdge(0, 10);
    toTest.addEdge(2, 3);
    REQUIRE(toTest.getEdges().size() == 2);
    REQUIRE(toTest.getDegree() == 13);

    toTest.delAllEdges();
    REQUIRE(toTest.getEdges().empty());
    REQUIRE(toTest.getDegree() == 0);
}