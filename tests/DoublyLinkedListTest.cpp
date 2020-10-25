//
// Created by tobias on 03.11.19.
//
#include "catch.h"

#include "../src/data_structures/DoublyLinkedList.h"
#include "IntegerFilter.h"

TEST_CASE("ConstructorTest", "[DoublyLinkedList]") {
    DoublyLinkedList<int> toTest = DoublyLinkedList<int>();
    REQUIRE(toTest.getSize() == 0);
    //REQUIRE(toTest.begin() == nullptr);
    //REQUIRE(toTest.end() == nullptr);
}

TEST_CASE("add test", "[DoublyLinkedList]") {
    DoublyLinkedList<int> toTest = DoublyLinkedList<int>();
    toTest.add(1);
    REQUIRE(toTest.getSize() == 1);
    REQUIRE((*toTest.begin()).getData() == 1);

    toTest.add(5);
    REQUIRE(toTest.getSize() == 2);
    REQUIRE((*toTest.begin()).getData() == 1);

    toTest.add(2);
    toTest.add(10);
    toTest.add(-4);
    REQUIRE(toTest.getSize() == 5);
    REQUIRE((*toTest.begin()).getData() == 1);
}

TEST_CASE("remove test", "[DoublyLinkedList]") {
    DoublyLinkedList<int> toTest = DoublyLinkedList<int>();
    toTest.add(0);
    toTest.add(3);
    toTest.add(-15);
    toTest.add(100);
    REQUIRE(toTest.getSize() == 4);

    bool result = toTest.remove(4);
    REQUIRE(toTest.getSize() == 4);
    REQUIRE(!result);
    REQUIRE((*toTest.begin()).getData() == 0);

    result = toTest.remove(0);
    REQUIRE(toTest.getSize() == 3);
    REQUIRE(result);
    REQUIRE((*toTest.begin()).getData() == 3);
    for (auto node:toTest) {
        CHECK(node.getData() != 0);
    }

    result = toTest.remove(3);
    REQUIRE(toTest.getSize() == 2);
    REQUIRE(result);
    REQUIRE((*toTest.begin()).getData() == -15);
    for (auto node:toTest) {
        CHECK(node.getData() != 3);
    }

    result = toTest.remove(100);
    REQUIRE(toTest.getSize() == 1);
    REQUIRE(result);
    REQUIRE((*toTest.begin()).getData() == -15);
    for (auto node:toTest) {
        CHECK(node.getData() != 100);
    }

    result = toTest.remove(-15);
    REQUIRE(toTest.getSize() == 0);
    REQUIRE(result);
    //REQUIRE(toTest.begin() == nullptr);
    //REQUIRE(toTest.end() == nullptr);
    for (auto node:toTest) {
        CHECK(node.getData() != -15);
    }
}

TEST_CASE("LoopTest", "[DoublyLinkedListTest]") {
    DoublyLinkedList<int> toTest = DoublyLinkedList<int>();
    toTest.add(0);
    toTest.add(1);
    toTest.add(2);
    int count = 0;
    for (auto e: toTest) {
        CHECK(e.getData() == count++);
    }

    auto l = DoublyLinkedList<int>();
    for (auto e: l) {
        CHECK(false);
    }
}

TEST_CASE("removeWithFilterTest", "[DoublyLinkedListTest]") {
    auto list = DoublyLinkedList<int>();
    list.add(0);
    list.add(4);
    list.add(21);
    list.add(-1);
    list.add(-200);
    list.add(400);
    //list: 0, 4, 21, -1, -200, 400
    auto filter = IntegerFilter(0, 4);
    auto rem = std::vector<int>();
    CHECK(list.remove(filter, rem));
    CHECK(list.getSize() == 4);
    CHECK(rem.size() == 2);
    unsigned int count = 0;
    auto expectedVec = std::vector<int>();
    auto expectedRemoved = std::vector<int>();
    expectedRemoved.emplace_back(0);
    expectedRemoved.emplace_back(4);
    expectedVec.emplace_back(21);
    expectedVec.emplace_back(-1);
    expectedVec.emplace_back(-200);
    expectedVec.emplace_back(400);
    for (auto node: list) {
        CHECK(node.getData() == expectedVec[count++]);
    }
    count = 0;
    for (auto i: rem) {
        CHECK(i == expectedRemoved[count++]);
    }

    //list: 21, -1, -200, 400
    filter = IntegerFilter(-200, -1);
    expectedVec = std::vector<int>();
    expectedRemoved = std::vector<int>();
    expectedRemoved.emplace_back(-1);
    expectedRemoved.emplace_back(-200);
    expectedVec.emplace_back(21);
    expectedVec.emplace_back(400);
    rem = std::vector<int>();
    count = 0;
    list.remove(filter, rem);
    CHECK(list.getSize() == 2);
    CHECK(rem.size() == 2);
    for (auto node: list) {
        CHECK(node.getData() == expectedVec[count++]);
    }
    count = 0;
    for (auto i: rem) {
        CHECK(i == expectedRemoved[count++]);
    }

    list.add(4);
    list.add(5);
    filter = IntegerFilter(4, 5);
    expectedVec = std::vector<int>();
    expectedRemoved = std::vector<int>();
    expectedRemoved.emplace_back(4);
    expectedRemoved.emplace_back(5);
    expectedVec.emplace_back(21);
    expectedVec.emplace_back(400);
    rem = std::vector<int>();
    count = 0;
    CHECK(list.remove(filter, rem));
    CHECK(list.getSize() == 2);
    CHECK(rem.size() == 2);
    for (auto node: list) {
        CHECK(node.getData() == expectedVec[count++]);
    }
    count = 0;
    for (auto i: rem) {
        CHECK(i == expectedRemoved[count++]);
    }

    list.add(3);
    list.add(500);
    list.add(350);
    list.add(6);
    list.add(7);
    list.add(351);
    //list: 21, 400, 3, 500, 350, 6, 7, 351

    filter = IntegerFilter(350, 500);
    expectedVec = std::vector<int>();
    expectedRemoved = std::vector<int>();
    expectedRemoved.emplace_back(400);
    expectedRemoved.emplace_back(500);
    expectedRemoved.emplace_back(350);
    expectedRemoved.emplace_back(351);
    expectedVec.emplace_back(21);
    expectedVec.emplace_back(3);
    expectedVec.emplace_back(6);
    expectedVec.emplace_back(7);
    rem = std::vector<int>();
    count = 0;
    CHECK(list.remove(filter, rem));
    CHECK(list.getSize() == 4);
    CHECK(rem.size() == 4);
    for (auto node: list) {
        CHECK(node.getData() == expectedVec[count++]);
    }
    count = 0;
    for (auto i: rem) {
        CHECK(i == expectedRemoved[count++]);
    }
}