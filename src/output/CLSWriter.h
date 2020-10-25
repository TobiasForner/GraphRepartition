//
// Created by Tobias on 19/11/2019.
//

#ifndef DBGTTHESIS_CLSWRITER_H
#define DBGTTHESIS_CLSWRITER_H

#include<iostream>
#include<fstream>

class CLSWriter {
public:
    CLSWriter(std::string filename) {
        file = std::ofstream(filename);
    }


    void writeToFile(std::string line) {
        file << line << std::endl;
    }


    void closeFile() {
        file.close();
    }

private:
    std::ofstream file;

};


#endif //DBGTTHESIS_CLSWRITER_H
