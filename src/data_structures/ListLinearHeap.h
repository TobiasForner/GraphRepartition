//
// Created by tobias on 23.10.19.
//

#include "vector"
#include <cassert>
#include <memory>
#include "DoublyLinkedList.h"
#include "ListLinearHeapElement.h"

#ifndef DBGTTHESIS_LISTLINEARHEAP_H
#define DBGTTHESIS_LISTLINEARHEAP_H

/**
 * class representing a data structure used or the decomposition tree
 */
class ListLinearHeap {
public:
    /**
     * constructor
     * @param n number of items to be managed at most
     * @param key_cap maximum key value
     */
    ListLinearHeap(const unsigned int n, const unsigned int key_cap) : elementsById(
            std::vector<DoublyLinkedListNode<ListLinearHeapElement *> *>(n, nullptr)),
                                                                       heads(std::vector<DoublyLinkedListNode<ListLinearHeapElement *> *>(
                                                                               key_cap + 1, nullptr)) {
        this->n = n;
        this->key_cap = key_cap;
        size = 0;
        max_key = 0;
    }

    /**
     * destructor
     */
    ~ListLinearHeap() {
        while (!isEmpty()) {
            unsigned int max_id;
            unsigned int max_k;
            extract_max(max_id, max_k);
        }
    }

    /**
     * inserts the pair (item, key)
     * @param item item to be inserted
     * @param key key to be given to the item
     */
    inline void insert(const unsigned int id, const unsigned int key) {
        assert(elementsById[id] == nullptr);
        assert(key <= key_cap);
        //construct new element
        auto newElem = new ListLinearHeapElement(id, key);
        auto newNode = new DoublyLinkedListNode<ListLinearHeapElement *>(newElem);
        newNode->setNext(heads[key]);
        if (heads[key] != nullptr) {
            heads[key]->setPre(newNode);
        }
        //insert new element
        heads[key] = newNode;
        elementsById[id] = newNode;
        //possibly update max_key
        if (key > max_key) {
            max_key = key;
        }
        size++;
    }

    /**
     * removes the specified element from the heap and returns the corresponding weight
     * @param id id of the element that is to be removed
     * @return weight of the removed element
     */
    inline unsigned int remove(const unsigned int id) {
        DoublyLinkedListNode<ListLinearHeapElement *> *toDelete = elementsById[id];
        assert(toDelete != nullptr);

        DoublyLinkedListNode<ListLinearHeapElement *> *next = toDelete->getNext();

        unsigned int weight = elementsById[id]->getData()->getKey();
        //check if head of a weight list
        if (toDelete->getPre() == nullptr) {
            heads[weight] = next;
        } else {
            //not head of a weight list
            DoublyLinkedListNode<ListLinearHeapElement *> *pre = toDelete->getPre();
            pre->setNext(next);

        }
        if (next != nullptr) {
            next->setPre(toDelete->getPre());
        }
        size--;
        delete (elementsById[id]->getData());
        delete (elementsById[id]);
        elementsById[id] = nullptr;
        return weight;
    }

    /**
     * inserts the id of the item with maximum key into max_id, the corresponding key into max_key and removes the item from the data structure
     * @param max_id
     * @param max_key
     * @return true if successful (not empty), false otw
     */
    inline bool extract_max(unsigned int &max_id, unsigned int &max_key_out) {
        if (isEmpty()) {
            return false;
        }

        while (heads[this->max_key] == nullptr) {
            --this->max_key;
            if (isEmpty()) {
                return false;
            }
        }
        max_id = heads[this->max_key]->getData()->getId();
        max_key_out = heads[this->max_key]->getData()->getKey();
        remove(max_id);

        return true;
    };

    /**
     * increments the key of an item by a given amount
     * @param id of the item whose key is to be increased
     * @param increment value by which the key is to be increased
     * @return new key value of item
     */
    inline unsigned int increment_key(const unsigned int id, const unsigned int increment) {
        assert(elementsById[id] != nullptr);
        unsigned int new_weight = elementsById[id]->getData()->getKey() + increment;
        assert(new_weight <= key_cap);
        remove(id);
        insert(id, new_weight);
        return new_weight;
    };

    /**
     * returns whether the data structure is empty
     * @return true if the heap is empty, false otherwise
     */
    inline bool isEmpty() const {
        return size == 0;
    }

private:
    unsigned int size;
    unsigned int n; //number of items
    unsigned int key_cap; //maximum key value
    unsigned int max_key;
    std::vector<DoublyLinkedListNode<ListLinearHeapElement *> *> elementsById;

    //pointers to doubly linked list containing weight equal to the index of heads that is looked at
    std::vector<DoublyLinkedListNode<ListLinearHeapElement *> *> heads;
};

#endif //DBGTTHESIS_LISTLINEARHEAP_H
