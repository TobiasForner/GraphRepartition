//
// Created by tobias on 24.10.19.
//
#include "catch.h"
#include "../src/data_structures/ListLinearHeap.h"

TEST_CASE("ListLinearHeap inserts correctly", "[ListLinearHeap]") {
    ListLinearHeap test = ListLinearHeap(5, 10);
    test.insert(0, 2);
    test.insert(1, 0);
    test.insert(4, 3);
    test.insert(3, 1);
    test.insert(2, 10);
    unsigned int key;
    unsigned int id;

    test.extract_max(id, key);
    REQUIRE(key == 10);
    REQUIRE(id == 2);

    test.extract_max(id, key);
    REQUIRE(key == 3);
    REQUIRE(id == 4);

    test.extract_max(id, key);
    REQUIRE(key == 2);
    REQUIRE(id == 0);

    test.extract_max(id, key);
    REQUIRE(key == 1);
    REQUIRE(id == 3);

    test.extract_max(id, key);
    REQUIRE(key == 0);
    REQUIRE(id == 1);
}

TEST_CASE("ListLinearHeap removes correctly", "[ListLinearHeap]") {
    ListLinearHeap test = ListLinearHeap(5, 10);
    test.insert(0, 2);
    test.insert(1, 0);
    test.insert(4, 3);
    test.insert(3, 1);
    test.insert(2, 10);

    unsigned int result;

    result = test.remove(0);
    REQUIRE(result == 2);

    result = test.remove(2);
    REQUIRE(result == 10);

    result = test.remove(4);
    REQUIRE(result == 3);

    result = test.remove(1);
    REQUIRE(result == 0);

    result = test.remove(3);
    REQUIRE(result == 1);
}

TEST_CASE("ListLinearHeap increment_key", "[ListLinearHeap]") {
    ListLinearHeap test = ListLinearHeap(5, 10);
    test.insert(0, 2);
    test.insert(1, 0);
    test.insert(4, 3);
    test.insert(3, 1);
    test.insert(2, 10);

    test.increment_key(4, 5);

    unsigned int key;
    unsigned int id;

    test.extract_max(id, key);

    REQUIRE(key == 10);
    REQUIRE(id == 2);
}