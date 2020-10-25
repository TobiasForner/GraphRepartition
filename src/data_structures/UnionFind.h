//
// Created by tobias on 17.12.19.
//


#ifndef DBGTTHESIS_UNIONFIND_H
#define DBGTTHESIS_UNIONFIND_H

#include <vector>
#include <stdexcept>

/**
 * class implementing a list-based union find data structure
 */
class UnionFind {
public:

    /**
     * constructor
     */
    UnionFind() {
        heads = std::vector<unsigned int>();
        next = std::vector<unsigned int>();
        size = std::vector<unsigned int>();
        containsElement = std::vector<bool>();
    }


    /**
     * adds x as a singleton to the set
     * @param x new element
     * @return true if x was added, false if x was already part of the structure
     */
    inline bool makeSet(const unsigned int x) {
        if (x < heads.size() && containsElement[x]) {
            return false;
        }
        bool ret = x >= heads.size() || !containsElement[x];
        while (x >= heads.size()) {
            heads.emplace_back(heads.size());
            next.emplace_back(next.size());
            size.emplace_back(1);
            containsElement.emplace_back(false);
        }
        containsElement[x] = true;
        return ret;
    }


    /**
     * returns a representative of the set x belongs to
     * @param x element that is to be found
     * @return returns representative of the set x belongs to
     */
    inline unsigned int find(const unsigned int x) const {
        if (x >= heads.size() || !containsElement[x]) {
            throw std::invalid_argument("Element " + std::to_string(x) + " not in union-find structure!");
        }
        return heads[x];
    }


    /**
     * replaces the sets of x and y with the union of the sets and returns the representative of the resulting set
     * @param x first element
     * @param y second element
     * @return representative of the union set
     */
    inline unsigned int unionSet(const unsigned int x, const unsigned int y) {
        if (heads.size() <= x || heads.size() <= y) {
            throw std::invalid_argument("Either x or y is not in the union-find structure.");
        }
        if (heads[x] == heads[y]) {
            return heads[x];
        }
        unsigned int smaller = size[heads[x]] < size[heads[y]] ? heads[x] : heads[y];
        unsigned int bigger = heads[x] + heads[y] - smaller;
        size[bigger] += size[smaller];
        auto current = smaller;
        while (true) {
            heads[current] = bigger;
            if (next[current] == current) {
                break;
            }
            current = next[current];
        }
        next[current] = next[bigger] != bigger ? next[bigger] : next[current];
        next[bigger] = smaller;
        return heads[bigger];
    }

    /**
     * returns the size of the set x belongs to
     * @param x
     * @return
     */
    inline unsigned int getSize(const unsigned int x) const {
        return size[heads[x]];
    }

    /**
     * returns the set represented by the identifier x as a vector
     * @param x identifier of the set
     * @return vector of the elements of the set
     */
    inline std::vector<unsigned int> getSetOf(const unsigned int x) const {
        auto result = std::vector<unsigned int>();
        auto current = heads[x];
        while (true) {
            result.emplace_back(current);
            if (current == next[current]) {
                break;
            }
            current = next[current];
        }
        return result;
    }

    /**
     * splits the set of x into singleton sets
     * @param x an element of the set that is to be split
     */
    inline void splitSetOf(const unsigned int x) {
        for (auto node: getSetOf(x)) {
            heads[node] = node;
            next[node] = node;
            size[node] = 1;
        }
    }

private:
    //std::vector<unsigned int> sizes;
    std::vector<unsigned int> heads;
    std::vector<unsigned int> next;
    std::vector<unsigned int> size;
    std::vector<bool> containsElement;

};


#endif //DBGTTHESIS_UNIONFIND_H
