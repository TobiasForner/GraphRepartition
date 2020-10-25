//
// Created by tobias on 03.11.19.
//

#include "catch.h"
#include "../src/data_structures/Tree.h"

TEST_CASE("Tree ConstructorTest", "[Tree]") {
    Tree<int> t = Tree<int>(-5);
    REQUIRE(t.getData() == -5);
    REQUIRE(t.getChildren().empty());

    std::vector<Tree<int> *> v;
    auto c1 = new Tree<int>(-1);
    auto c2 = new Tree<int>(123);
    v.emplace_back(c1);
    v.emplace_back(c2);

    Tree<int> t2 = Tree<int>(v, 1);
    REQUIRE(t2.getData() == 1);
    REQUIRE(t2.getChildren().size() == 2);
    REQUIRE(t2.getChildren()[0]->getData() == -1);
    REQUIRE(t2.getChildren()[1]->getData() == 123);
}