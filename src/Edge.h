//
// Created by tobias on 24.10.19.
//

#ifndef DBGTTHESIS_EDGE_H
#define DBGTTHESIS_EDGE_H

#include <iostream>
#include "data_structures/DoublyLinkedList.h"
#include "typedefs.h"

namespace decomposition_tree {

    class Edge {
    private:
        weight_ weight;
        node_ start;
        node_ end;
        Edge *reverse;
    public:
        /**
         * Constructor
         * @param start start node
         * @param end end node
         * @param weight weight of the edge
         */
        Edge(const node_ start, const node_ end, const weight_ weight) : weight(weight), start(start), end(end),
                                                                         reverse(nullptr) {}

        /**
         * destructor
         */
        ~Edge() {
            reverse = nullptr;
        };

        /**
         * Increases the weight of the edge by w
         * @param w weight that is to be added
         */
        inline void increase_weight(const weight_ w) {
            this->weight += w;
        }

        /**
         * returns the weight of the edge
         * @return weight of the edge
         */
        inline weight_ getWeight() const {
            return weight;
        }

        /**
         * returns the start of the edge
         * @return start of the edge
         */
        inline node_ getStart() const {
            return start;
        }

        /**
         * returns the end of the edge
         * @return end of the edge
         */
        inline node_ getEnd() const {
            return end;
        }

        /**
         * returns pointer to the corresponding reverse edge
         * @return pointer to the corresponding reverse edge
         */
        inline Edge *getReverse() const {
            return reverse;
        }

        /**
         * sets the reverse edge
         * @param rev new reverse edge
         */
        inline void setReverse(Edge *rev) {
#ifdef USE_DEBUG
            if (rev != nullptr) {
                if (rev->getStart() != this->getEnd() || rev->getEnd() != this->start || rev->getWeight() != weight) {
                    std::cerr << "Invalid reverse edge" << std::endl;
                }
            }
#endif
            reverse = rev;
        };

        /**
         * sets the end point of the edge
         * @param target new endpoint
         */
        inline void setTarget(const node_ target) {
            end = target;
        }
    };
}

#endif //DBGTTHESIS_EDGE_H