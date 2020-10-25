//
// Created by tobias on 25.12.19.
//

#include "catch.h"

#include "../src/ClusterMapping.h"
#include "../src/check_cluster_mapping.h"


TEST_CASE("updateMappingTestNothingChanges", "[ClusterMappingTest]") {
    auto mapping = ClusterMapping(6, 2, 3, 2.1, 0.1, true);
    auto firstMapping = mapping.getMapping();
    CHECK(decomposition_tree::check_mapping(firstMapping, 3, 4));
    auto components = std::vector<std::vector<decomposition_tree::node_ >>();
    for (unsigned int i = 0; i < 6; i++) {
        components.emplace_back(std::vector<decomposition_tree::node_>(1, i));
    }
    auto toDel = std::vector<std::vector<decomposition_tree::node_ >>();
    bool migration = false;
    mapping.updateMapping(components, toDel, migration);
    auto secondMapping = mapping.getMapping();

    REQUIRE(toDel.empty());
    REQUIRE(firstMapping.size() == 6);
    REQUIRE(secondMapping.size() == 6);
    for (unsigned int i = 0; i < firstMapping.size(); i++) {
        REQUIRE(firstMapping[i] == secondMapping[i]);
    }
    CHECK(decomposition_tree::check_mapping(secondMapping, 3, 4));
    CHECK(mapping.getMigrationNr() == 0);
    REQUIRE(!migration);
}

TEST_CASE("updateMappingTest with deletion", "[ClusterMappingTest]") {
    auto mapping = ClusterMapping(6, 2, 3, 2.1, 0.1, true);
    auto firstMapping = mapping.getMapping();
    CHECK(decomposition_tree::check_mapping(firstMapping, 3, 4));
    auto components = std::vector<std::vector<decomposition_tree::node_ >>();
    components.emplace_back(std::vector<decomposition_tree::node_>(1, 0));
    components.emplace_back(std::vector<decomposition_tree::node_>(1, 1));
    components.emplace_back(std::vector<decomposition_tree::node_>(1, 2));
    auto compToDel = std::vector<decomposition_tree::node_>();
    compToDel.emplace_back(3);
    compToDel.emplace_back(4);
    compToDel.emplace_back(5);
    components.emplace_back(compToDel);
    auto toDel = std::vector<std::vector<decomposition_tree::node_ >>();
    bool migration = false;
    mapping.updateMapping(components, toDel, migration);
    auto secondMapping = mapping.getMapping();

    REQUIRE(toDel.size() == 1);
    REQUIRE(firstMapping.size() == 6);
    REQUIRE(secondMapping.size() == 6);
    for (unsigned int i = 0; i < firstMapping.size(); i++) {
        REQUIRE(firstMapping[i] == secondMapping[i]);
    }
    CHECK(decomposition_tree::check_mapping(secondMapping, 3, 4));
    CHECK(mapping.getMigrationNr() == 0);
}

TEST_CASE("updateMappingTestWithMigration", "[ClusterMappingTest]") {
    auto mapping = ClusterMapping(6, 2, 3, 2.1, 0.1, true);
    auto firstMapping = mapping.getMapping();
    CHECK(decomposition_tree::check_mapping(firstMapping, 3, 4));
    //find two nodes that are mapped to different clusters and merge them into one component
    auto components = std::vector<std::vector<decomposition_tree::node_ >>();
    auto compToMerge = std::vector<decomposition_tree::node_>();
    compToMerge.emplace_back(0);
    bool notfound = true;
    for (unsigned int i = 0; i < firstMapping.size(); i++) {
        if (notfound && firstMapping[i] != firstMapping[0]) {
            notfound = false;
            compToMerge.emplace_back(i);
        } else if (i != 0) {
            components.emplace_back(std::vector<decomposition_tree::node_>(1, i));
        }
    }
    REQUIRE(compToMerge.size() == 2);
    REQUIRE(firstMapping[compToMerge[0]] != firstMapping[compToMerge[1]]);
    components.emplace_back(compToMerge);
    auto toDel = std::vector<std::vector<decomposition_tree::node_ >>();
    bool migration = false;
    mapping.updateMapping(components, toDel, migration);
    auto secondMapping = mapping.getMapping();

    REQUIRE(toDel.empty());
    REQUIRE(firstMapping.size() == 6);
    REQUIRE(secondMapping.size() == 6);
    REQUIRE(secondMapping[compToMerge[0]] == secondMapping[compToMerge[1]]);
    REQUIRE(mapping.getMigrationNr() == 1);
    CHECK(decomposition_tree::check_mapping(secondMapping, 3, 4));
    CHECK(mapping.getMigrationNr() == 1);
}

