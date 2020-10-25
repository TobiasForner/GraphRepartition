//
// Created by tobias on 23.10.19.
//

#include "DoublyLinkedListNode.h"
#include "DoublyLinkedListIterator.h"
#include "../Filter.h"
#include <vector>

#include <memory>

#ifndef DBGTTHESIS_DOUBLYLINKEDLIST_H
#define DBGTTHESIS_DOUBLYLINKEDLIST_H

template<class T>
class DoublyLinkedList {
public:
    /**
     * Constructor
     */
    DoublyLinkedList() {
        //spdlog::debug("Creating DoublyLinkedList");
        head = nullptr;
        tail = nullptr;
        size = 0;
    }

    DoublyLinkedList(const DoublyLinkedList<T> &other) = delete;

    /**
     * move constructor
     * @param other
     */
    DoublyLinkedList(DoublyLinkedList<T> &&other) {
        this->head = other.head;
        this->tail = other.tail;
        this->size = other.size;
        other.head = nullptr;
        other.tail = nullptr;
        other.size = 0;
    }

    /**
     * Destructor
     */
    ~DoublyLinkedList() {
        //spdlog::debug("Deleting DoublyLinkedList");

        while (head != nullptr) {
            remove(head->getData());
        }
    }

    /**
     * adds a new DoublyLinkedListNode with data elem to the end of the list
     * @param elem
     */
    void add(const T elem) {
        if (head == nullptr) {
            head = new DoublyLinkedListNode<T>(elem, nullptr, nullptr);
            tail = head;
        } else {
            auto *newTail = new DoublyLinkedListNode<T>(elem, tail, nullptr);
            tail->setNext(newTail);
            tail = newTail;
        }
        size++;
    };

    /**
     * removes elem from the list if the list contains it
     * @param elem element to be removed
     * @return true if elem was in the list, false otw
     */
    bool remove(const T elem) {
        if (head != nullptr && head->getData() == elem) {
            if (head == tail) {
                tail = nullptr;
            }
            auto oldHead = head;
            head = head->getNext();
            delete (oldHead);
            if (head != nullptr) {
                head->setPre(nullptr);
            }
            size--;
            return true;
        }
        DoublyLinkedListNode<T> *node = head;

        while (node != nullptr) {
            if (node->getData() == elem) {
                if (node->getPre() != nullptr) {
                    node->getPre()->setNext(node->getNext());
                }
                if (node->getNext() != nullptr) {
                    node->getNext()->setPre(node->getPre());
                }
                if (node == tail) {
                    tail = node->getPre();
                }
                size--;
                delete node;
                return true;
            }
            node = node->getNext();
        }
        return false;
    }

    /**
     * removes all elements for which the filter is true
     * @param f filter
     * @param removed vector of removed elements
     * @return true if at least one element was removed, false otherwise
     */
    bool remove(const Filter<T> &f, std::vector<T> &removed) {
        if (size == 0) {
            return false;
        }
        while (head != nullptr && f(head->getData())) {
            removed.emplace_back(head->getData());
            remove(head->getData());
        }
        if (size == 0) {
            return removed.size() > 0;
        }

        auto currentNode = head->getNext();
        DoublyLinkedListNode<T> *lastNodeNotFiltered = head;

        while (currentNode != nullptr) {
            while (f(currentNode->getData())) {
                size--;
                removed.emplace_back(currentNode->getData());
                if (currentNode == tail) {
                    tail = lastNodeNotFiltered;
                    tail->setNext(nullptr);
                    return removed.size() > 0;
                }
                currentNode = currentNode->getNext();
                delete (currentNode->getPre());
            }
            lastNodeNotFiltered->setNext(currentNode);
            currentNode->setPre(lastNodeNotFiltered);
            lastNodeNotFiltered = currentNode;
            /*
            if (currentNode == nullptr) {
                break;
            }*/
            currentNode = currentNode->getNext();
        }

        return removed.size() > 0;
    }

    /**
     * returns the first node in the list, nullptr if the list is empty
     * @return first node in the list
     */
    DoublyLinkedListNode<T> *getFront() const {
        return head;
    }

    /**
     * returns start iterator
     * @return start iterator
     */
    DoublyLinkedListIterator<T> begin() const {
        return DoublyLinkedListIterator<T>(head);
    }

    /**
     * returns end iterator
     * @return end iterator
     */
    DoublyLinkedListIterator<T> end() const {
        return DoublyLinkedListIterator<T>(nullptr);
    }

    /**
     * returns the size of the list
     * @return size of the list
     */
    unsigned int getSize() const {
        return size;
    }

private:
    unsigned int size;
    DoublyLinkedListNode<T> *head;
    DoublyLinkedListNode<T> *tail;
};

#endif //DBGTTHESIS_DOUBLYLINKEDLIST_H
