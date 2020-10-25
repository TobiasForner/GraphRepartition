//
// Created by tobias on 12.01.20.
//

#include "../src/data_sources/CSVDataSource.h"
#include "catch.h"

TEST_CASE("testReadsInputFileCorrectly", "[CSVDataSourceTest]") {
    CSVDataSource toTest = CSVDataSource("tests/test-input/test_csv_data.csv");

    std::vector<std::pair<decomposition_tree::node_, decomposition_tree::node_ >> expected;
    expected.emplace_back(21, 56);
    expected.emplace_back(75, 1);
    expected.emplace_back(8, 0);
    expected.emplace_back(3, 123);

    std::vector<decomposition_tree::Edge> results;
    while (toTest.hasNext()) {
        results.emplace_back(toTest.getNext());
    }
    REQUIRE(expected.size() == results.size());

    for (unsigned int i = 0; i < results.size(); i++) {
        CHECK(results[i].getStart() == expected[i].first);
        CHECK(results[i].getEnd() == expected[i].second);
        CHECK(results[i].getWeight() == 1);
    }
}

