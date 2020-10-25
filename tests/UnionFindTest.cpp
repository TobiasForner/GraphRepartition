//
// Created by tobias on 25.12.19.
//

#include "catch.h"
#include "../src/data_structures/UnionFind.h"

TEST_CASE("UnionFindTest", "[UnionFindTest]") {
    auto uf = UnionFind();
    REQUIRE(uf.makeSet(1));
    REQUIRE(!uf.makeSet(1));
    CHECK(uf.find(1) == 1);
    CHECK_THROWS(uf.find(10));
    REQUIRE(uf.makeSet(0));
    REQUIRE(uf.getSize(1) == 1);
    REQUIRE(uf.getSize(0) == 1);
    REQUIRE(uf.makeSet(5));
    REQUIRE(uf.makeSet(3));
    REQUIRE(uf.makeSet(2));
    REQUIRE(uf.makeSet(4));
    REQUIRE(uf.getSize(2) == 1);
    REQUIRE(uf.getSize(3) == 1);
    REQUIRE(uf.getSize(4) == 1);
    REQUIRE(uf.getSize(5) == 1);
    uf.unionSet(0, 1);
    CHECK(uf.find(1) == 0);
    CHECK(uf.find(0) == 0);
    REQUIRE(uf.getSize(0) == 2);
    REQUIRE(uf.getSize(1) == 2);
    REQUIRE(uf.getSetOf(0).size() == 2);
    REQUIRE(uf.getSetOf(1).size() == 2);
    uf.unionSet(4, 5);
    uf.unionSet(3, 2);
    uf.unionSet(3, 4);
    REQUIRE(uf.getSize(2) == 4);
    REQUIRE(uf.getSize(3) == 4);
    REQUIRE(uf.getSize(4) == 4);
    REQUIRE(uf.getSize(5) == 4);
    REQUIRE(uf.getSetOf(2).size() == 4);
    REQUIRE(uf.getSetOf(3).size() == 4);
    REQUIRE(uf.getSetOf(4).size() == 4);
    REQUIRE(uf.getSetOf(5).size() == 4);
    std::vector<bool> checks = std::vector<bool>(6, false);
    for (auto i: uf.getSetOf(2)) {
        checks[i] = true;
    }
    REQUIRE(checks[0] == false);
    REQUIRE(checks[1] == false);
    REQUIRE(checks[2] == true);
    REQUIRE(checks[3] == true);
    REQUIRE(checks[4] == true);
    REQUIRE(checks[5] == true);

    uf.makeSet(7);
    CHECK_THROWS(uf.find(6));
}


TEST_CASE("splitSetOfTest", "UnionFindTest") {
    UnionFind uf = UnionFind();
    uf.makeSet(10);
    uf.makeSet(5);
    uf.makeSet(1);
    uf.makeSet(3);
    uf.unionSet(10, 5);
    uf.unionSet(10, 1);
    uf.unionSet(10, 3);
    uf.splitSetOf(10);
    CHECK(uf.getSize(10) == 1);
    CHECK(uf.getSetOf(10).size() == 1);
    CHECK(uf.getSize(5) == 1);
    CHECK(uf.getSetOf(5).size() == 1);
    CHECK(uf.getSize(1) == 1);
    CHECK(uf.getSetOf(1).size() == 1);
    CHECK(uf.getSize(3) == 1);
    CHECK(uf.getSetOf(3).size() == 1);
}

