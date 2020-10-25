//
// Created by tobias on 03.12.19.
//

#ifndef DBGTTHESIS_CONFIGREADER_H
#define DBGTTHESIS_CONFIGREADER_H


#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>

class AttributeNotFoundException : public std::exception {
public:
    explicit AttributeNotFoundException(const std::string &attr) {
        this->message = "AttributeNotFoundException: attribute " + attr + " was not found";;
    }

    const char *what() const noexcept override {
        return message.c_str();
    }

private:
    std::string message;
};

class ConfigReader {
public:
    /**
     * Constructor
     */
    ConfigReader();

    void readConfigOrConfigList(const std::string &filename);

    bool setToNextConfig();

    /**
     * reads attribute name, value pairs into the map
     * @param filename
     */
    void readConfig(const std::string &filename);

    /**
     * Attempts to retrieve string value of the attribute from the map, throws AttributeNotFoundException if not found
     * @param attrName name of the attribute
     * @return value if defined, throws AttributeNotFoundException otherwise
     */
    std::string getVal(const std::string &attrName) const;

    /**
     * Attempts to retrieve int value of the attribute from the map, throws AttributeNotFoundException if not found
     * @param attrName name of the attribute
     * @return value if defined, throws AttributeNotFoundException otherwise
     */
    int getIntVal(const std::string &attrName) const;

    /**
     * Attempts to retrieve bool value of the attribute from the map, throws AttributeNotFoundException if not found
     * @param attrName name of the attribute
     * @return value if defined, throws AttributeNotFoundException otherwise
     */
    bool getBoolVal(const std::string &attrName) const;

    /**
     * Attempts to retrieve double value of the attribute from the map, throws AttributeNotFoundException if not found
     * @param attrName name of the attribute
     * @return value if defined, throws AttributeNotFoundException otherwise
     */
    double getDoubleVal(const std::string &attrName) const;

private:
    //map for storing the attribute name, value pairs
    std::vector<std::map<std::string, std::string>> values;
    int values_num;

};


#endif //DBGTTHESIS_CONFIGREADER_H
