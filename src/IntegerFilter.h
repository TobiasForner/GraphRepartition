//
// Created by tobias on 24.02.20.
//
#include "Filter.h"

#ifndef DBGTTHESIS_INTEGERFILTER_H
#define DBGTTHESIS_INTEGERFILTER_H

class IntegerFilter : public Filter<int> {
public:
    /**
     * constructor
     * @param lowerBound lower bound on integers the filter is to accept
     * @param upperBound upper bound on integers the filter is to accept
     */
    IntegerFilter(const int lowerBound, const int upperBound) : lower(lowerBound), upper(upperBound) {
    }

    /**
     * destructor
     */
    ~IntegerFilter() override = default;

    /**
     * filter function
     * @param obj object that is to be filtered
     * @return true if obj is between lower and upper (both inclusively), false otherwise
     */
    inline bool operator()(int obj) const override {
        return obj >= lower && obj <= upper;
    }

private:
    int lower;
    int upper;
};

#endif //DBGTTHESIS_INTEGERFILTER_H
