//
// Created by tobias on 29.02.20.
//
#include "Edge.h"
#include "Filter.h"

#ifndef DBGTTHESIS_EDGEFILTERCOREHALO_H
#define DBGTTHESIS_EDGEFILTERCOREHALO_H

class EdgeFilterCoreHalo : public Filter<decomposition_tree::Edge *> {
public:
    /**
     * constructor
     * @param startIndex index of the first position that is to be filtered
     * @param endIndex index of the last position that is to be filtered
     * @param nodeToPos maps nodes to their position
     */
    EdgeFilterCoreHalo(const unsigned int startIndex, const unsigned int endIndex,
                       const std::vector<unsigned int> &nodeToPos) : startIndex(startIndex), endIndex(endIndex),
                                                                     nodeToPos(nodeToPos) {}

    /**
    * specifies whether a given edge passes the filter
    * @param e edge that is to be filtered
    * @return true if e passes, false otherwise
    */
    inline bool operator()(decomposition_tree::Edge *e) const override {
        auto start = nodeToPos[e->getStart()];
        auto end = nodeToPos[e->getEnd()];
        return (startIndex <= start && endIndex >= start) || (startIndex <= end && endIndex >= end);
    };

private:
    unsigned int startIndex;
    unsigned int endIndex;
    const std::vector<unsigned int> &nodeToPos;

};

#endif //DBGTTHESIS_EDGEFILTERCOREHALO_H
