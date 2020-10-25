//
// Created by tobias on 21.10.19.
//
/**
 * Class representing the inner nodes of the decomposition tree
 */

#ifndef DBGTTHESIS_GRAPHNODE_H
#define DBGTTHESIS_GRAPHNODE_H

#include "iostream"

class TreeNode {
public:
    /**
     * basic constructor
     */
    TreeNode() {
        startIndex = 0;
        endIndex = 0;
        connectivity = 0;
        if (startIndex > endIndex) {
            std::cerr << "Error: tried to create invalid TreeNode!" << std::endl;
            exit(1);
        }
    }


    /**
     * constructor
     * @param start start of the leaf list segment represented by this node
     * @param end end of the tree leaf segment represented by this node
     * @param connectivity connectivity of the sub-graph represented by this node
     */
    TreeNode(const unsigned int start, const unsigned int end, const unsigned int connectivity) {
        startIndex = start;
        endIndex = end;
        this->connectivity = connectivity;
        if (startIndex > endIndex) {
            std::cerr<<"Error: tried to create invalid TreeNode!"<<std::endl;
            exit(1);
        }
    }


    /**
     * returns the connectivity of the sub-graph represented by this node, or alpha if the connectivity is larger
     * @return connectivity set for this node
     */
    inline unsigned int getConnectivity() const {
        return connectivity;
    }


    /**
     * sets the connectivity
     * @param c new connectivity
     */
    inline void setConnectivity(unsigned int c) {
        connectivity = c;
    }


    /**
     * returns the start of the leaf list segment represented by this node
     * @return start of the leaf list segment represented by this node
     */
    inline unsigned int getStartIndex() const {
        return startIndex;
    }


    /**
     * sets a new start index
     * @param start new start index
     */
    inline void setStartIndex(unsigned int start) {
        this->startIndex = start;
    }


    /**
     * returns the end of the leaf list segment represented by this node
     * @return end of the leaf list segment represented by this node
     */
    inline unsigned int getEndIndex() const {
        return endIndex;
    }


    /**
     * sets a new end index
     * @param end new end index
     */
    inline void setEndIndex(unsigned int end) {
        this->endIndex = end;
    }

private:
    /**
     * the start index of the section in the node list corresponding to this subgraph
     */
    unsigned int startIndex;

    /**
     * the start index of the section in the node list corresponding to this subgraph
     */
    unsigned int endIndex;

    /**
     * the connectivity of the sub-graph represented by this node
     */
    unsigned int connectivity;

};


#endif //DBGTTHESIS_GRAPHNODE_H
