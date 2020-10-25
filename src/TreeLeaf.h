//
// Created by tobias on 22.10.19.
//



#include "typedefs.h"

#ifndef DBGTTHESIS_TREELEAF_H
#define DBGTTHESIS_TREELEAF_H
namespace decomposition_tree {
    /**
     * class representing the leaves of a decomposition tree, each node corresponds to a single vertex of the original graph
     */
    class TreeLeaf {
    public:
        /**
         * constructor
         * @param nr number of the node represented in the leaf
         */
        explicit TreeLeaf(unsigned int nr){
            number = nr;
        }

        /**
         * returns the number of the node represented by the leaf
         * @return number of the node represented by the leaf
         */
        inline node_ getNumber() const{
            return number;
        };

        /**
         * sets the number of the leaf/ the node represented by it
         * @param nr new number
         */
        inline void setNumber(const node_ nr){
            this->number = nr;
        }

    private:
        //number of the node represented by this leaf
        node_ number;

    };
}


#endif //DBGTTHESIS_TREELEAF_H
