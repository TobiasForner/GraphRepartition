//
// Created by tobias on 03.11.19.
//

#include "catch.h"
#include "../src/TreeLeaf.h"

namespace decomposition_tree {

    TEST_CASE("TreeLeaf ConstructorTest", "[TreeLeaf]") {
        TreeLeaf t = TreeLeaf(1);
        REQUIRE(t.getNumber() == 1);
    }

    TEST_CASE("TreeLeaf getter and setter", "[TreeLeaf]") {
        TreeLeaf t = TreeLeaf(1);
        t.setNumber(5);
        REQUIRE(t.getNumber() == 5);
    }
}

