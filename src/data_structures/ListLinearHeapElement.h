//
// Created by tobias on 23.10.19.
//

#ifndef DBGTTHESIS_LISTLINEARHEAPELEMENT_H
#define DBGTTHESIS_LISTLINEARHEAPELEMENT_H

/**
 * class representing the elements of the ListLinearHeap
 */
class ListLinearHeapElement {
private:
    unsigned int id;
    unsigned int key;
public:
    /**
     * base constructor
     */
    ListLinearHeapElement() : id(0), key(0) {};

    /**
     * constructor
     * @param id id of the element
     * @param key key of the element
     */
    ListLinearHeapElement(const unsigned int id, const unsigned int key) : id(id), key(key) {};

    /**
     * returns the key
     * @return key of the element
     */
    inline unsigned int getKey() const {
        return key;
    };

    /**
     * returns the id of the element
     * @return id of the element
     */
    inline unsigned int getId() const {
        return id;
    }
};

#endif //DBGTTHESIS_LISTLINEARHEAPELEMENT_H
