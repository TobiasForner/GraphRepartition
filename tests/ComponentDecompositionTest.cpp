//
// Created by Tobias on 24/03/2020.
//

#include "catch.h"
#include "../src/ComponentDecomposition.h"

TEST_CASE("mapping is updated correctly", "[ComponentDecompositionTest]") {
    auto toTest = decomposition_tree::ComponentDecomposition(9, 2, 2.1, 3, 3, false);
    decomposition_tree::STEP_ACTION stepAction;
    long ms = 0;
    toTest.insertAndUpdate(0, 3, stepAction, ms);
    toTest.insertAndUpdate(0, 3, stepAction, ms);
    REQUIRE(toTest.getMapping()[0] == toTest.getMapping()[3]);

    toTest.insertAndUpdate(0, 6, stepAction, ms);
    toTest.insertAndUpdate(0, 6, stepAction, ms);
    REQUIRE(toTest.getMapping()[0] == toTest.getMapping()[6]);

    toTest.insertAndUpdate(1, 4, stepAction, ms);
    toTest.insertAndUpdate(1, 4, stepAction, ms);
    REQUIRE(toTest.getMapping()[1] == toTest.getMapping()[4]);
    toTest.insertAndUpdate(1, 7, stepAction, ms);
    toTest.insertAndUpdate(1, 7, stepAction, ms);
    REQUIRE(toTest.getMapping()[1] == toTest.getMapping()[7]);

    toTest.insertAndUpdate(2, 5, stepAction, ms);
    toTest.insertAndUpdate(2, 5, stepAction, ms);
    REQUIRE(toTest.getMapping()[2] == toTest.getMapping()[5]);
    toTest.insertAndUpdate(2, 8, stepAction, ms);
    toTest.insertAndUpdate(2, 8, stepAction, ms);
    REQUIRE(toTest.getMapping()[2] == toTest.getMapping()[8]);


    toTest.insertAndUpdate(0, 1, stepAction, ms);
    toTest.insertAndUpdate(0, 1, stepAction, ms);
    toTest.insertAndUpdate(0, 1, stepAction, ms);
    toTest.insertAndUpdate(0, 1, stepAction, ms);

    REQUIRE(toTest.getMapping()[0] == toTest.getMapping()[1]);
}

