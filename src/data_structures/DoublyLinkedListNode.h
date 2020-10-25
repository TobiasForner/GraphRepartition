//
// Created by tobias on 23.10.19.
//

#ifndef DBGTTHESIS_DOUBLYLINKEDLISTNODE_H
#define DBGTTHESIS_DOUBLYLINKEDLISTNODE_H

#include <memory>
//#include "spdlog/spdlog.h"

template<class T>
class DoublyLinkedListNode {
private:
    DoublyLinkedListNode *pre;
    DoublyLinkedListNode *next;
    T data;
public:
    /**
     * Constructor
     * @param data data to be stored in this DoublyLinkedListNode
     */
    explicit DoublyLinkedListNode(const T data) : pre(nullptr), next(nullptr), data(data) {
    }

    /**
     * Constructor
     * @param data data to be stored in this DoublyLinkedListNode
     * @param pre predecessor in the list
     * @param next next node in the list
     */
    DoublyLinkedListNode(const T data, DoublyLinkedListNode *pre, DoublyLinkedListNode *next) : pre(pre),
                                                                                                next(next), data(data) {
    }

    /**
     * Destructor
     */
    ~DoublyLinkedListNode() {
        //spdlog::debug("Deleting DoublyLinkedListNode.");
        pre = nullptr;
        next = nullptr;
    }

    /**
     * returns predecessor
     * @return pointer to predecessor
     */
    inline DoublyLinkedListNode *getPre() const {
        return pre;
    }

    /**
     * returns pointer to next node
     * @return pointer to next node
     */
    inline DoublyLinkedListNode *getNext() const {
        return next;
    }

    /**
     * returns reference to data contained in this node
     * @return reference to data contained in this node
     */
    inline T &getData() {
        return data;
    }

    /**
     * returns whether the node has a next element
     * @return true if the node has a next element, false otherwise
     */
    bool hasNext() const {
        return next != nullptr;
    }

    /**
     * sets the next node
     * @param newNext new next node
     */
    void setNext(DoublyLinkedListNode *newNext) {
        next = newNext;
    }

    /**
     * sets the predecessor
     * @param newPre new predecessor
     */
    void setPre(DoublyLinkedListNode *newPre) {
        pre = newPre;
    }

    /**
     * operator for use with iterator-based for loops
     * @return
     */
    DoublyLinkedListNode<T> *operator++() {
        return next;
    }

};

#endif //DBGTTHESIS_DOUBLYLINKEDLISTNODE_H
