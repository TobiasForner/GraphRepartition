//
// Created by tobias on 07.01.20.
//

#include "DoublyLinkedListNode.h"

#ifndef DBGTTHESIS_DOUBLYLINKEDLISTITERATOR_H
#define DBGTTHESIS_DOUBLYLINKEDLISTITERATOR_H

/**
 * class for iterating through the doubly linked list nodes of parameter type T
 * @tparam T the type of the data elements contained in the DoublyLinkedList
 */
template<class T>
class DoublyLinkedListIterator {
public:
    /**
     * constructor
     * @param p pointer this iterator should start at
     */
    explicit DoublyLinkedListIterator(DoublyLinkedListNode<T> *p) {
        pointer = p;
    }

    /**
     * compares two iterators
     * @param other iterator to compare with
     * @return true if both iterators manage the same pointer, false otherwise
     */
    bool operator==(const DoublyLinkedListIterator<T> &other) const {
        return other.pointer == this->pointer;
    }

    /**
     * compares two iterators
     * @param other iterator to compare with
     * @return false if both iterators manage the same pointer, true otherwise
     */
    bool operator!=(const DoublyLinkedListIterator<T> &other) const {
        return other.pointer != this->pointer;

    }

    /**
     * changes the managed pointer to the next one
     * @return pointer to the updated iterator
     */
    DoublyLinkedListIterator<T> &operator++() {
        pointer = pointer->getNext();
        return *this;
    }

    /**
     * post increment of the iterator
     * @return current state of the iterator
     */
    DoublyLinkedListIterator<T> operator++(int) {
        auto temp(*this);
        pointer = pointer->getNext();
        return temp;
    }

    /**
     * returns the node managed by this iterator
     * @return node managed by this iterator
     */
    DoublyLinkedListNode<T> &operator*() const {
        return *pointer;
    }

protected:
    DoublyLinkedListNode<T> *pointer;
};

#endif //DBGTTHESIS_DOUBLYLINKEDLISTITERATOR_H
