//
// Created by tobias on 24.10.19.
//



#ifndef DBGTTHESIS_GRAPH_H
#define DBGTTHESIS_GRAPH_H

#include "vector"
#include "../Edge.h"
#include "DoublyLinkedList.h"

namespace decomposition_tree {

    /**
     * class representing an undirected graph
     */
    class Graph {
    private:
        //number of nodes (dynamic), the graph always contains the nodes 0,...,n-1
        unsigned int n;
        //number of edges (dynamic)
        unsigned int m;

        //edges for each node, vector of adjacencies represented by edges
        std::vector<std::vector<Edge *>> edges;

    public:
        /**
         * constructor
         * @param n number of initial nodes
         */
        explicit Graph(unsigned int n);

        /**
         * destructor
         */
        ~Graph();

        /**
         * adds nodes up until node (inclusive)
         * @param node
         */
        void addNode(node_ node);

        /**
         * adds edge e to the graph or increases weight of the corresponding edge as well as the reverse edge, adds nodes if they are not yet in the graph
         * @param e edge to be added
         */
        void addEdge(const Edge &e);

        /**
         * removes all edges connected to node n
         * @param n node
         */
        void removeAllEdgesOfNode(node_ n);

        /**
         * returns the number of nodes
         * @return number of nodes
         */
        inline unsigned int getNodesNr() const {
            return n;
        }

        /**
         * returns the number of edges
         * @return number of edges
         */
        inline unsigned int getEdgesNr() const {
            return m;
        }

        /**
         * returns the edges adjacent to node i
         * @param i node whose edges are to be returned
         * @return edges of node i
         */
        inline std::vector<Edge *> getEdges(node_ i) const {
            if (i >= edges.size()) {
                throw std::range_error("Node index " + std::to_string(i) + " out of range.");
            }
            return edges[i];
        }

    };
}


#endif //DBGTTHESIS_GRAPH_H
