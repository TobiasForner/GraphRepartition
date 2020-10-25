//
// Created by tobias on 11.01.20.
//

#ifndef DBGTTHESIS_CSVDATASOURCE_H
#define DBGTTHESIS_CSVDATASOURCE_H

#include <fstream>
#include <sstream>
#include "DataSource.h"


class CSVDataSource : public DataSource {
public:
    /**
     * constructor
     * @param filename name of the file that is to be read
     */
    explicit CSVDataSource(const std::string &filename);

    /**
     * specifies whether there are unread lines in the ile
     * @return true if their are unread lines, false otherwise
     */
    bool hasNext() override;

    /**
     * interprets the values in the next line as an edge of weight one and returns it
     * @return edge representation of the values in the current line
     */
    decomposition_tree::Edge getNext() override;

    /**
     * prints info about the current status
     */
    void printInfo() override;

private:
    std::ifstream file;
};


#endif //DBGTTHESIS_CSVDATASOURCE_H
