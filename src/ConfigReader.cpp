//
// Created by tobias on 03.12.19.
//

#include "ConfigReader.h"

ConfigReader::ConfigReader() {
    values = std::vector<std::map<std::string, std::string>>();
    values.emplace_back(std::map<std::string, std::string>());
    values_num = 0;
}

void ConfigReader::readConfigOrConfigList(const std::string &filename) {
    std::ifstream file;
    file.open(filename);
    std::string line;
    if (std::getline(file, line)) {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        if (line == "config_list") {
            while (std::getline(file, line)) {
                line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
                if (this->values_num != 0) {
                    this->values.emplace_back(std::map<std::string, std::string>());
                }
                this->readConfig(line);
                this->values_num += 1;
            }
            file.close();
        } else {
            file.close();
            readConfig(filename);
        }
    }
    this->values_num = 0;
}


bool ConfigReader::setToNextConfig() {
    if (values_num + 1 < values.size()) {
        values_num += 1;
        return true;
    } else {
        return false;
    }
}

void ConfigReader::readConfig(const std::string &filename) {
    std::ifstream file;
    file.open(filename);
    if (!file) {
        throw std::invalid_argument("File " + filename + " could not be opened");
    }
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        std::istringstream tmp(line);
        std::string attr;
        if (std::getline(tmp, attr, '=')) {
            std::string val;
            if (std::getline(tmp, val)) {
                values[values_num].emplace(attr, val);
            }
        } else {
            throw std::invalid_argument(
                    "Attributes and Values must be separated by an equal sign without enclosing whitespaces! This is not the case in line " +
                    std::to_string(lineNumber));
        }
    }
    file.close();
}

std::string ConfigReader::getVal(const std::string &attrName) const {
    if (values[values_num].find(attrName) != values[values_num].end()) {
        return values[values_num].find(attrName)->second;
    }
    throw AttributeNotFoundException(attrName);
}

int ConfigReader::getIntVal(const std::string &attrName) const {
    if (values[values_num].find(attrName) != values[values_num].end()) {
        return std::stoi(values[values_num].find(attrName)->second);
    }
    throw AttributeNotFoundException(attrName);
}

bool ConfigReader::getBoolVal(const std::string &attrName) const {
    if (values[values_num].find(attrName) != values[values_num].end()) {
        return std::stoi(values[values_num].find(attrName)->second) == 1;
    }
    throw AttributeNotFoundException(attrName);
}

double ConfigReader::getDoubleVal(const std::string &attrName) const {
    if (values[values_num].find(attrName) != values[values_num].end()) {
        return std::stod(values[values_num].find(attrName)->second);
    }
    throw AttributeNotFoundException(attrName);
}