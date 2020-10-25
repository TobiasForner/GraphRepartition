//
// Created by tobias on 24.01.20.
//

#ifndef DBGTTHESIS_EDGEFILTERCORE_H
#define DBGTTHESIS_EDGEFILTERCORE_H

#include "Filter.h"
#include "Edge.h"


class EdgeFilterCore : public Filter<decomposition_tree::Edge *> {
public:
    /**
     * constructor
     * @param startIndex first index that should pass the filter
     * @param endIndex last index that should pass the filter
     * @param indirectionVec vector specifying indirections
     */
    EdgeFilterCore(const unsigned int startIndex, const unsigned int endIndex,
                   const std::vector<unsigned int> &indirectionVec) : startIndex(startIndex), endIndex(endIndex),
                                                                      indirectionVec(indirectionVec) {}

    /**
     * specifies whether a given edge passes the filter
     * @param e edge that is to be filtered
     * @return true if e passes, false otherwise
     */
    inline bool operator()(decomposition_tree::Edge *e) const override {
        return (indirectionVec[e->getStart()] >= startIndex && indirectionVec[e->getStart()] <= endIndex &&
                indirectionVec[e->getEnd()] >= startIndex && indirectionVec[e->getEnd()] <= endIndex);
    }

private:
    unsigned int startIndex;
    unsigned int endIndex;
    const std::vector<unsigned int> &indirectionVec;

};


#endif //DBGTTHESIS_EDGEFILTERCORE_H
