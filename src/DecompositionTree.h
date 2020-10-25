//
// Created by tobias on 21.10.19.
//

#include "data_structures/Tree.h"
#include "TreeNode.h"
#include "TreeLeaf.h"
#include "Edge.h"
#include "MergeComponent.h"
#include "data_structures/ListLinearHeap.h"
#include "DecompositionMethod.h"
#include "ClusterMapping.h"
#include "check_cluster_mapping.h"
#include "EdgeFilterCore.h"
#include "EdgeFilterCoreHalo.h"

#ifdef LOGGING_ENABLED
#include "spdlog/spdlog.h"
#endif

#include <queue>
#include <sstream>
#include <chrono>

#ifndef DBGTTHESIS_DECOMPOSITIONTREE_H
#define DBGTTHESIS_DECOMPOSITIONTREE_H

namespace decomposition_tree {

    class DecompositionTree : public DecompositionMethod {

    public:
        //allows to specify which method is used to delete edges: the old version to delete core only, or new
        // filter-based deletions that either only delete the core or also the halo
        enum DELETION_METHOD {
            OLD, CORE_ONLY_FILTER, CORE_HALO_FILTER
        };

        enum COMP_GRAPH_CONSTRUCTION {
            NAIVE, ALPHA_COMP, CHILDREN
        };

        /**
         * Constructor for decomposition tree with n nodes and no edges
         * @param n number of nodes in the original graph
         * @param alpha alpha for the decomposition algorithm; this corresponds to the highest connectivity that is checked
         * @param augmentation augmentation factor, should be of the form 2 + epsilon for the algorithm to work, with epsilon positive
         * @param clusterSize size of the individual clusters
         * @param clusterNr number of clusters
         * @param initRandomly specifies whether the internal data structures should be initialized randomly
         * @param considerMapping specifies whether all edges should be inserted or only those between nodes that are mapped to different clusters
         * @param tryOpt specifies whether experimental optimization techniques should be used
         * @param del_method specifies the deletion method
         * @param insertBetweenServers specifies whether edges should only be inserted between different servers or also between components
         */
        explicit DecompositionTree(unsigned int n, unsigned int alpha, double augmentation,
                                   unsigned int clusterSize, unsigned int clusterNr, bool initRandomly, bool tryOpt,
                                   DELETION_METHOD del_method = OLD, COMP_GRAPH_CONSTRUCTION= NAIVE,
                                   bool insertBetweenServers = true);

        /**
         * constructor for decomposition tree via a Graph with n nodes and edge lists for each node
         * @param n number of nodes
         * @param alpha alpha for the decomposition algorithm; this corresponds to the highest connectivity that is checked
         * @param edges vector of edge lists for each node
         * @param augmentation augmentation factor, should be of the form 2 + epsilon for the algorithm to work, with epsilon positive
         * @param clusterSize size of the individual clusters
         * @param clusterNr number of clusters
         * @param initRandomly specifies whether the internal data structures should be initialized randomly
         * @param considerMapping specifies whether all edges should be inserted or only those between nodes that are mapped to different clusters
         * @param tryOpt specifies whether experimental optimization techniques should be used
         * @param del_method specifies the deletion method
         * @param insertBetweenServers specifies whether edges should only be inserted between different servers or also between components
         */
        DecompositionTree(node_ n, unsigned int alpha, std::vector<DoublyLinkedList<Edge *>> &edges,
                          double augmentation, unsigned int clusterSize, unsigned int clusterNr, bool initRandomly,
                          bool tryOpt, DELETION_METHOD del_method = OLD, COMP_GRAPH_CONSTRUCTION= NAIVE,
                          bool insertBetweenServers = true);

        /**
         * destructor
         */
        ~DecompositionTree() override;


        /**
         * add an edge between a and b
         * @param a first node
         * @param b second node
         */
        void addEdge(node_ a, node_ b) override;

        /**
         * returns the smallest sub-graph in the decomposition tree that contains the nodes a nd b
         * @param a first node
         * @param b second node
         * @return smallest sub-graph that contains both a and b
         */
        Tree<TreeNode *> *findSmallestSubgraph(node_ a, node_ b);

        /**
         * update decomposition of the smallest sub-graph in the tree that contains a and b, the highest connectivity set for any sub-graph is upper bounded by alpha
         * @param a first node
         * @param b second node
         */
        void updateDecomposition(node_ a, node_ b) override;

        /**
         * inserts an edge and updates the internal structures accordingly, this has a few checks to make it more
         * efficient as compared to separate calls of addEdge and updateDecomposition
         * @param a first node of the edge
         * @param b second node of the edge
         */
        void insertAndUpdate(node_ a, node_ b, STEP_ACTION &stepAction, long &ms) override;

        /**
         * returns the mapping as currently maintained by the decomposition tree data structure
         * @return current mapping
         */
        std::vector<unsigned int> getMapping() const override;

        /**
         * Prints general run info
         */
        void printInfo() const override;

        /**
         * Returns the root of the decomposition tree
         * @return root of the decomposition tree
         */
        Tree<TreeNode *> *getRoot();

        /**
         * Returns the edges of the decomposition tree
         * @return vector of doubly linked lists representing the edges of each node
         */
        std::vector<std::shared_ptr<DoublyLinkedList<Edge *>>> getEdges();

        /**
         * Returns the alpha value set for the decomposition tree
         * @return alpha value
         */
        node_ getAlpha() {
            return alpha;
        }

        /**
        * returns the total time needed for the main update algorithm of the decomposition tree up until this point
        * @return time passed in milliseconds
        */
        long getMilliseconds() const override;

    private:
        enum COMPONENT_STATE {
            INSERTED, REMOVED, OUTSIDE
        };
        //alpha used as upper bound for the decomposition connectivity
        unsigned int alpha;

        bool tryOpt;

        bool insertBetweenServers{true};

        bool gatherStepStats;

        DELETION_METHOD deletionMethod;

        COMP_GRAPH_CONSTRUCTION compGraphConstruction;
        //root of the decomposition tree
        Tree<TreeNode *> *root;
        //node list; these are the leaves of the tree
        std::vector<TreeLeaf> leaves;
        //maps each node to the corresponding leaf index
        std::vector<node_> nodesToLeaves;
        //adjacencies of each node
        std::vector<std::shared_ptr<DoublyLinkedList<Edge *>>> edges;

        std::string edgeCountFile;

#ifdef COLLECT_STATS
        //the number of updates which did not change anything because both communication endpoints were mapped to the same cluster
        unsigned int nrSkipped;

        unsigned int nrDeleted;


        //remembers the level of the returned tree node per findSmallestSubgraph call
        std::vector<unsigned int> accessLevelCount;
#endif

        /**
         * This method can be used to check invariants of the tree, used for debugging
         * Currently it checks the following
         * -for each TreeNode: start<=end
         * -weights nd start/target consistency of edges that are each other's reverse
         * -reverse edges are in the correct lists
         */
        void checkWholeTree() const {
            auto check_queue = std::queue<Tree<TreeNode *> *>();
            check_queue.push(root);
            while (!check_queue.empty()) {
                auto tmp = check_queue.front();
                check_queue.pop();
                if (tmp->getData()->getStartIndex() > tmp->getData()->getEndIndex()) {
                    std::cerr << "ERROR: indices invalid" << std::endl;
                }
                if (tmp->getData()->getConnectivity() >= alpha && !tmp->getChildren().empty()) {
                    std::cerr << "ERROR: tree node with connectivity alpha has children!" << std::endl;
                    exit(1);
                }
                for (auto c:tmp->getChildren()) {
                    if (c->getData()->getConnectivity() <= tmp->getData()->getConnectivity()) {
                        std::cerr << "Connectivity does not increase with level!" << std::endl;
                        exit(1);
                    }
                    check_queue.push(c);
                }
            }

            //check all edges
            unsigned int i = 0;
            for (const auto &edgeList:edges) {
                for (auto e:*edgeList) {
                    if (e.getData()->getStart() != e.getData()->getReverse()->getEnd() ||
                        e.getData()->getEnd() != e.getData()->getReverse()->getStart() ||
                        e.getData()->getWeight() != e.getData()->getReverse()->getWeight()) {
                        std::cerr << "edge data error" << std::endl;
                        exit(1);
                    }
                    if (e.getData()->getStart() != i) {
                        std::cerr << "Edge in wrong list!" << std::endl;
                        exit(1);
                    }
                }
                i++;
            }
            for (const auto &edgeList:edges) {
                for (auto listNode: *edgeList) {
                    auto e = listNode.getData();
                    bool found = false;
                    for (auto revSearch: *edges[e->getEnd()]) {
                        if (revSearch.getData()->getEnd() == e->getStart() &&
                            revSearch.getData()->getWeight() == e->getWeight()) {
                            found = true;
                        }
                    }
                    if (!found) {
                        std::cerr << "Edge does not exist in lists of both end points" << std::endl;
                        exit(1);
                    }
                }
            }
        }

        //vector of alpha connected components as discovered by the algorithm, this should be accurate after an updateDecomposition call
        std::vector<std::vector<node_>> alphaConnectedComponents;

        ClusterMapping clusterMapping;

        /**
         * recomputes the alpha connected components of the tree based on its current state and puts them into the target
         * @param target vector the components are to be inserted into, this is expected to be empty
         * @return nothing
         */
        void recomputeAlphaConnectedComponents(std::vector<std::vector<node_>> &target);

        /**
         * computes the alpha connected components of the sub-graph represented by the startTree and inserts them into the target,
         * requires the internal structures to be up-to-date, otherwise single nodes are returned for parts where no TreeNode with connectivity of at least alpha exists
         * @param startTree root of the tree tht is to be searched for alpha connected components
         * @param target target vector the components are to be inserted to, this is expected to be empty
         * @return nothing
         */
        void computeAlphaConnectedComponents(Tree<TreeNode *> *startTree,
                                             std::vector<std::vector<node_>> &target) const;

        /**
         * deletes the specified component, i.e. it turns it into singletons, deletes all internal edges and recomputes the decomposition
         * @param comp component that is to be deleted
         */
        void delAlphaConnectedComponent(std::vector<node_> comp);

        /**
         * Constructs a graph of MergeComponents from TreeNode tN
         * @param tree representing the input sub-graph
         * @param components vector of components that is to be filled
         */
        void constructComponentGraph(const Tree<TreeNode *> *tree, std::vector<MergeComponent *> &components) const;

        /**
         * swaps nodes a nd b in the leaf list
         * @param a first node
         * @param b second node
         */
        void swap(node_ a, node_ b);

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
         * @param components vector of all components
         */
        void mergeComponents(MergeComponent *m1, MergeComponent *m2, std::vector<bool> &validForNextIt,
                             std::vector<MergeComponent *> &components) const;

        /**
         * basic decomposition step
         * @param tree tree representing the sub-graph that is to be decomposed
         * @param newConnectivity value of the new connectivity that tN is to be decomposed with
         * @param smallestCut reference to a field that decompose sets to the smallest cut it has encountered;
         * this can be used to set the connectivity of the sub-graph correctly
         * @return vector of TreeNodes that tN was decomposed into, each TreeNode with connectivity at least newConnectivity
         */
        std::vector<TreeNode *>
        decomposition(const Tree<TreeNode *> *tree, unsigned int newConnectivity, unsigned int &smallestCut);

        /**
         * recursively deletes the tree data in the tree rooted at t
         * @param t root of the tree
         */
        static void deleteTreeData(Tree<TreeNode *> *t);

        /**
         * sets root to contain the whole of the leaves and sets its connectivity to zero
         */
        void resetRoot();

        /**
         * Returns whether tN contains node n
         * @param tN tree node that is to be exmined
         * @param n graph node that is to be examined
         * @return true if tN contains n, false otherwise
         */
        bool containsNode(const TreeNode *tN, node_ n) const;
    };
}


#endif //DBGTTHESIS_DECOMPOSITIONTREE_H
