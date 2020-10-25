//
// Created by tobias on 03.11.19.
//

#include "catch.h"
#include "../src/TreeNode.h"

TEST_CASE("TreeNode ConstructorTest", "[TreeNode]") {
    TreeNode t = TreeNode();
    REQUIRE(t.getConnectivity() == 0);
    REQUIRE(t.getStartIndex() == 0);
    REQUIRE(t.getEndIndex() == 0);

    TreeNode t2 = TreeNode(0, 1, 10);
    REQUIRE(t2.getEndIndex() == 1);
    REQUIRE(t2.getStartIndex() == 0);
    REQUIRE(t2.getConnectivity() == 10);
}

TEST_CASE("TreeNode getter and setter", "[TreeNode]") {
    TreeNode t2 = TreeNode(0, 1, 10);
    REQUIRE(t2.getEndIndex() == 1);
    REQUIRE(t2.getStartIndex() == 0);
    REQUIRE(t2.getConnectivity() == 10);
    t2.setConnectivity(50);
    REQUIRE(t2.getConnectivity() == 50);

}

