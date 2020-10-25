//
// Created by tobias on 21.10.19.
//
/**
 * Basic recursive tree data structure
 */
#include "vector"
#include "iostream"
#include "DoublyLinkedList.h"

#ifndef DBGTTHESIS_TREE_H
#define DBGTTHESIS_TREE_H

/**
 * class defining a tree (node) that contains elements of type T
 * @tparam T type of the tree elements
 */
template<class T>
class Tree {
public:
    /**
     * constructor
     * @param data data object to be contained in this tree object
     */
    explicit Tree(const T data) {
        children = std::vector<Tree<T> *>();
        this->data = data;
    }

    /**
     * basic destructor
     */
    ~Tree() {
        for (auto t: children) {
            delete (t);
        }
    }

    /**
     * constructor
     * @param children children of this tree
     * @param data data object to be contained in this tree object
     */
    Tree(std::vector<Tree<T> *> children, const T data) {
        this->children = children;
        this->data = data;
    }

    /**
     *returns the data stored in this tree
     * @return data stored in the current node
     */
    inline T getData() const {
        return data;
    };

    /**
     * sets the children of this tree node
     * @param newChildren
     */
    inline void setChildren(const std::vector<Tree<T> *> newChildren) {
        children = newChildren;
    }

    /**
     * set the data of this node
     * @param d new data
     */
    inline void setData(const T d) {
        this->data = d;
    }

    /**
     * returns the children of the node
     * @return children list of the current node
     */
    inline std::vector<Tree<T> *> getChildren() const {
        return children;
    };

private:
    //children vector
    std::vector<Tree<T> *> children;
    //data stored in current node
    T data;
};

#endif //DBGTTHESIS_TREE_H
