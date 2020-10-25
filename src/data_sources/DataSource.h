//
// Created by Tobias on 15/11/2019.
//

#ifndef DBGTTHESIS_DATASOURCE_H
#define DBGTTHESIS_DATASOURCE_H

#include "../Edge.h"

/**
 * abstract class defining an interface for data sources
 */
class DataSource {
public:

    /**
     * virtual destructor
     */
    virtual ~DataSource() = default;

    /**
     * gives information about whether there is another entry in the data source
     * @return true if there is another entry, false otherwise
     */
    virtual bool hasNext() = 0;

    /**
     * gets the next edge that is in the data source, only call after hasNext has returned true
     * @return next Edge
     */
    virtual decomposition_tree::Edge getNext() = 0;

    /**
     * prints some information about the data that was provided
     */
    virtual void printInfo() = 0;
};

#endif //DBGTTHESIS_DATASOURCE_H
