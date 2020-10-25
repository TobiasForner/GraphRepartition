//
// Created by tobias on 03.01.20.
//

#include "catch.h"
#include "../src/ConfigReader.h"

TEST_CASE("ConfigReaderTest", "[ConfigReaderTest]") {
    ConfigReader configReader = ConfigReader();
    configReader.readConfig("tests/test-input/testConfig.txt");
    CHECK_THROWS(configReader.getIntVal("nakjnjakbjkbjakbja"));
    CHECK(configReader.getIntVal("a") == 100);
    CHECK(configReader.getVal("b") == "abcd");
    CHECK(configReader.getDoubleVal("testTest123") == 123.1);
}

