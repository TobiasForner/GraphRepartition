//
// Created by tobias on 24.02.20.
//

#include "IntegerFilter.h"
#include "catch.h"

TEST_CASE("IntegerFilterTest", "[IntegerFilterTest]") {
    auto filter = IntegerFilter(-1, 4);
    CHECK(filter(3));
    CHECK(filter(-1));
    CHECK(filter(4));
    CHECK_FALSE(filter(-100));
    CHECK_FALSE(filter(2001));
}
