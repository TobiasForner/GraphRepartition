//
// Created by tobias on 11.01.20.
//

#include "CSVDataSource.h"

CSVDataSource::CSVDataSource(const std::string &filename) {
    file.open(filename);
    if (!file) {
        throw std::runtime_error("Error while trying to open file " + filename + "!");
    }
}

bool CSVDataSource::hasNext() {
    return file.peek() != EOF;
}

decomposition_tree::Edge CSVDataSource::getNext() {
    std::string line;
    if (!std::getline(file, line)) {
        throw std::runtime_error("Error while reading line in input!");
    }
    std::istringstream tmp(line);
    std::string src;
    std::string dst;
    if (!std::getline(tmp, src, ',')) {
        throw std::runtime_error("Error while getting first line element!");
    }
    if (!std::getline(tmp, dst)) {
        throw std::runtime_error("Error while getting destination from line!");
    }
    return decomposition_tree::Edge((unsigned int) std::stoi(src), (unsigned int) std::stoi(dst), 1);
}

void CSVDataSource::printInfo() {
    std::cout << "CSVDataSource info: empty for now.\n";
}
