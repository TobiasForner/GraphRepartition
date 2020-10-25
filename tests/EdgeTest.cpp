//
// Created by tobias on 30.10.19.
//

#define CATCH_CONFIG_MAIN

#include "catch.h"

#include "../src/Edge.h"

namespace decomposition_tree {

    TEST_CASE("Edge Constructor and getter Test", "[Edge]") {
        Edge e = Edge(0, 1, 5);
        REQUIRE(e.getStart() == 0);
        REQUIRE(e.getEnd() == 1);
        REQUIRE(e.getWeight() == 5);
        REQUIRE(e.getReverse() == nullptr);
    }

    TEST_CASE("increase_weight Test", "[Edge]") {
        Edge e = Edge(0, 1, 5);
        REQUIRE(e.getWeight() == 5);
        e.increase_weight(0);
        REQUIRE(e.getWeight() == 5);
        e.increase_weight(2);
        REQUIRE(e.getWeight() == 7);
    }


    TEST_CASE("setReverse Test", "[Edge]") {
        Edge e = Edge(2, 3, 1);
        Edge *reverse = new Edge(3, 2, 1);
        e.setReverse(reverse);
        REQUIRE(e.getReverse() != nullptr);
        REQUIRE(e.getReverse() == reverse);
        //delete (reverse);
    }


    TEST_CASE("setTarget Test", "[Edge]") {
        Edge e = Edge(2, 3, 1);
        REQUIRE(e.getEnd() == 3);
        e.setTarget(0);
        REQUIRE(e.getEnd() == 0);
    }

}

