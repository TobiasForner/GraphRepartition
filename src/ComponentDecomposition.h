//
// Created by Tobias on 22/03/2020.
//
#include "typedefs.h"
#include "ClusterMapping.h"
#include "DecompositionMethod.h"
#include "data_structures/Graph.h"
#include "MergeComponent.h"
#include "data_structures/ListLinearHeap.h"
#include <exception>
#include <queue>

#ifndef DBGTTHESIS_COMPONENTDECOMPOSITION_H
#define DBGTTHESIS_COMPONENTDECOMPOSITION_H
namespace decomposition_tree {

    class ComponentDecomposition : public DecompositionMethod {
    public:
        ComponentDecomposition(unsigned int n, unsigned int alpha, double augmentation, unsigned int clusterSize,
                               unsigned int clusterNr, bool initRandomly);

        ~ComponentDecomposition() override;


        /**
     * add edge between a and b to the internal data structures
     * @param a first node of the edge
     * @param b second node of the edge
     */
        void addEdge(decomposition_tree::node_ a, decomposition_tree::node_ b) override;

        /**
         * updates the decomposition after an insertion of the nodes a and b
         * @param a first node
         * @param b second node
         */
        void updateDecomposition(decomposition_tree::node_ a, decomposition_tree::node_ b) override;

        /**
         * this method combines add edge and update decomposition
         * @param a first node of the added edge
         * @param b second node of the added edge
         * @param stepAction specifies what step action was performed
         */
        void insertAndUpdate(decomposition_tree::node_ a, decomposition_tree::node_ b, STEP_ACTION& stepAction, long &ms) override;

        /**
         * Returns the current mapping as maintained by the decomposition method
         * @return current mapping
         */
        std::vector<unsigned int> getMapping() const override;

        /**
         * Prints general information about the run like the total communication and migration costs
         */
        void printInfo() const override;

        /**
         * returns the total time needed for the main update algorithm of the decomposition method up until this point
         * @return time passed in milliseconds
         */
        long getMilliseconds() const override;

    private:
        enum COMPONENT_STATE {
            INSERTED, REMOVED, OUTSIDE
        };
        unsigned int alpha;
        decomposition_tree::Graph g;
        ClusterMapping clusterMapping;
        std::vector<std::vector<node_>> components;

        void deleteComponent(std::vector<node_> &comp);

        void decomposition(std::vector<bool> &validForNextIt, std::vector<MergeComponent *> &currComponents,
                           unsigned int &nrResult);

        /**
         * cuts the component m from all other components
         * @param m MergeComponent that is to be cut
         * @param components vector of all components
         */
        void cutComponent(MergeComponent *m, std::vector<MergeComponent *> &components) const;

        /**
         * Merges component m2 into m1
         * @param m1 first component
         * @param m2 second component
         * @param validForNextIt states which components are valid for the next loop iteration in decomposition
         * @param currComponents vector of all components
         */
        void mergeComponents(MergeComponent *m1, MergeComponent *m2, std::vector<bool> &validForNextIt,
                             std::vector<MergeComponent *> &currComponents) const;

        /**
         * Constructs a graph of MergeComponents from TreeNode tN
         * @param tree representing the input sub-graph
         * @param currComponents vector of components that is to be filled
         */
        void constructComponentGraph(std::vector<MergeComponent *> &currComponents,
                                     std::vector<unsigned int> &nodesToMergeComp) const;

        void checkComponents(std::vector<std::vector<node_>> comps);
    };
}


#endif //DBGTTHESIS_COMPONENTDECOMPOSITION_H
