//
// Created by tobias on 04.12.19.
//

#include "typedefs.h"
#include <chrono>
#include "Edge.h"

#ifndef DBGTTHESIS_DECOMPOSITIONMETHOD_H
#define DBGTTHESIS_DECOMPOSITIONMETHOD_H

/**
 * abstract class defining an interface for decomposition methods
 */
class DecompositionMethod {
public:

    /**
     * virtual destructor
     */
    virtual ~DecompositionMethod() {};

    /**
     * add edge between a and b to the internal data structures
     * @param a first node of the edge
     * @param b second node of the edge
     */
    virtual void addEdge(const decomposition_tree::node_ a, const decomposition_tree::node_ b) = 0;

    /**
     * updates the decomposition after an insertion of the nodes a and b
     * @param a first node
     * @param b second node
     */
    virtual void updateDecomposition(const decomposition_tree::node_ a, const decomposition_tree::node_ b) = 0;

    /**
     * this method combines add edge and update decomposition
     * @param a first node of the added edge
     * @param b second node of the added edge
     * @param stepAction way for the decomposition method to return what happened during the update step
     * @param ms time the update step took in ms
     */
    virtual void
    insertAndUpdate(const decomposition_tree::node_ a, const decomposition_tree::node_ b,
                    decomposition_tree::STEP_ACTION &stepAction, long &ms) = 0;

    /**
     * Returns the current mapping as maintained by the decomposition method
     * @return current mapping
     */
    virtual std::vector<unsigned int> getMapping() const = 0;

    /**
     * Prints general information about the run like the total communication and migration costs
     */
    virtual void printInfo() const = 0;

    /**
     * returns the total time needed for the main update algorithm of the decomposition method up until this point
     * @return time passed in milliseconds
     */
    virtual long getMilliseconds() const = 0;


protected:
    std::chrono::microseconds totalTimePassed;
};

#endif //DBGTTHESIS_DECOMPOSITIONMETHOD_H
