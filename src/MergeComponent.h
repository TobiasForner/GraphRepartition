//
// Created by tobias on 28.10.19.
//
#include "vector"

#include "Edge.h"

#ifndef DBGTTHESIS_MERGECOMPONENT_H
#define DBGTTHESIS_MERGECOMPONENT_H
namespace decomposition_tree {

    /**
     * class for representing (possibly merged) nodes and their edges
     */
    class MergeComponent {
    public:
        /**
         * constructor
         * @param compNr number of the component
         * @param nodeNr number of the first node
         */
        MergeComponent(const id_ compNr, const id_ nodeNr) {
            number = compNr;
            nodes = std::vector<id_>();
            nodes.emplace_back(nodeNr);
            edges = std::vector<Edge *>();
            degree = 0;
        }

        /**
         * destructor
         */
        ~MergeComponent() {
            delAllEdges();
        }

        /**
         * adds a node to the component, checks if node is already in the list
         * @param nodeNr number of the new node
         */
        inline void addNode(const id_ nodeNr) {
            for (unsigned int i:nodes) {
                if (i == nodeNr) {
                    return;
                }
            }
            nodes.emplace_back(nodeNr);
        }

        /**
         * adds a node to the component, does not check if it is already in the list
         * @param nodeNr number of the new node
         */
        inline void addNodeNoChecks(const id_ nodeNr) {
            nodes.push_back(nodeNr);
        }

        /**
         * adds an edge to the component and adjusts the degree accordingly, checks if the edge is already present
         * @param e edge that is to be added
         */
        inline void addEdge(Edge *e) {
            if (e->getEnd() == number) {
                return;
            }
            //search edges
            for (Edge *tmp: edges) {
                if (tmp->getEnd() == e->getEnd()) {
                    return;
                }
            }
            edges.emplace_back(e);
            degree += e->getWeight();
        }

        /**
         * adds an edge to the component and adjusts the degree accordingly, does not check if the edge is already in the list
         * @param e
         */
        inline void addEdgeNoChecks(Edge *e) {
            edges.emplace_back(e);
            degree += e->getWeight();
        }

        /**
         * add edge with specified target to the edge vector edges, only adds the edge if there is no edge with the same target
         * @param target number of the target component, this number is its position in the component list
         * @param weight weight of the edge that is to be added
         */
        inline void addEdge(const id_ target, const weight_ weight) {
            Edge *tmp = new Edge(number, target, weight);
            addEdge(tmp);
        }

        /**
         * merges edge to current edge set: either add new edge or increase weight, deletes e if it is not inserted into the edge list
         * @param target the other end point of the edge
         * @param weight weight of the edge that is to be merged
         */
        void mergeEdge(Edge *e) {
            if (e->getEnd() == number) {
                delete (e);
                return;
            }
            for (Edge *eSelf:edges) {
                if (eSelf->getEnd() == e->getEnd()) {
                    eSelf->increase_weight(e->getWeight());
                    degree += e->getWeight();
                    //delete e if it is found
                    delete (e);
                    return;
                }
            }
            edges.emplace_back(e);
            degree += e->getWeight();
        }

        /**
         * searches for an edge with the specified target and deletes it
         * @param target
         */
        weight_ delEdge(const id_ target) {
            for (unsigned int i = 0; i < edges.size(); i++) {
                if (edges[i]->getEnd() == target) {
                    weight_ weight = edges[i]->getWeight();
                    degree -= edges[i]->getWeight();
                    edges[i]->setReverse(nullptr);
                    delete (edges[i]);
                    edges.erase(edges.begin() + i);
                    return weight;
                }
            }
            return 0;
        }

        /**
         * Deletes all edges in this component; reverse edges need to be handled separately
         */
        inline void delAllEdges() {
            for (unsigned int i = 0; i < edges.size(); i++) {
                edges[i]->setReverse(nullptr);
                delete (edges[i]);
            }
            edges = std::vector<Edge *>();
            degree = 0;
        }

        /**
         * returns the ids of all nodes in this component
         * @return vector of ids of the nodes in this component
         */
        inline std::vector<id_> getNodes() const {
            return nodes;
        }

        /**
         * returns all edges of this component
         * @return edges of the component
         */
        inline std::vector<Edge *> &getEdges() {
            return edges;
        }

        /**
         * retuns the number of this component
         * @return number of this component
         */
        inline id_ getNumber() const {
            return number;
        }

        /**
         * returns the degree of this component
         * @return degree of this component
         */
        inline weight_ getDegree() const {
            return degree;
        }

    private:
        std::vector<id_> nodes;
        std::vector<Edge *> edges;
        id_ number;
        weight_ degree;
    };
}

#endif //DBGTTHESIS_MERGECOMPONENT_H

