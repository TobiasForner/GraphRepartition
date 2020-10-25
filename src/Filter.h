//
// Created by tobias on 24.01.20.
//

#ifndef DBGTTHESIS_FILTER_H
#define DBGTTHESIS_FILTER_H

/**
 * a basic  abstract class for filtering objects
 * @tparam T type of the objects the filter is compatible with
 */
template<class T>
class Filter {
public:
    /**
     * virtual destructor
     */
    virtual ~Filter() {};

    /**
     * filter operation
     * @param obj object that is to be filtered
     * @return true if obj passes, false otherwise
     */
    virtual bool operator()(const T obj) const = 0;
};

#endif //DBGTTHESIS_FILTER_H
