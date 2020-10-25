//
// Created by tobias on 10.12.19.
//
#include "DataSource.h"

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include "mongocxx/instance.hpp"
#include "mongocxx/collection.hpp"
#include "mongocxx/exception/exception.hpp"
#include <bsoncxx/builder/stream/document.hpp>
#include <unordered_map>

#ifdef LOGGING_ENABLED
#include "spdlog/spdlog.h"
#endif

#include <chrono>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

#ifndef DBGTTHESIS_MONGODBDATAEXPERIMENTAL_H
#define DBGTTHESIS_MONGODBDATAEXPERIMENTAL_H


class MongoDBDataExperimental : public DataSource {
public:
    /**
     * Constructor
     * @param uri uri string for the mongodb
     * @param db name of the database
     * @param collection name of the collection
     * @param batchSize size of the batches that are queried at once
     * @param maxNumV maximal number of vertices that are to be in the resulting graph
     * @param sortBy name of the field to be sorted by, leave empty for no sorting
     * @param srcName name of the source of the communication in the database entries
     * @param dstName name of the source of the communication in the database entries
     */
    MongoDBDataExperimental(const std::string &uri, const std::string &db, const std::string &collection, int batchSize,
                            int maxNumV, std::string sortBy, std::string srcName, std::string dstName);

    /**
     * destructor
     */
    ~MongoDBDataExperimental() override;

    /**
     * specifies whether there is still data that was not returned
     * @return true if there is still data, false otherwise
     */
    inline bool hasNext() override {
        return nextFound;
    }

    /**
     * returns the next edge
     * @return next edge
     */
    inline decomposition_tree::Edge getNext() override {
        std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();
        if (!nextFound) {
            throw std::runtime_error("No next element!");
        }
        decomposition_tree::Edge ret = next;
        nextFound = findNext();
        nrReturned++;
        std::chrono::time_point<std::chrono::high_resolution_clock> end = std::chrono::high_resolution_clock::now();
        timePassed += std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return ret;
    }

    /**
     * prints basic info of the data source
     */
    inline void printInfo() override {
#ifdef LOGGING_ENABLED
        spdlog::info("Total: " + std::to_string(nrReturned + nrSkipped) + ", returned: " + std::to_string(nrReturned) +
                     ", skipped: " + std::to_string(nrSkipped) + ", number of docs examined: " +
                     std::to_string(nrDocsExamined) + ", total DB time: " + std::to_string(timePassed.count()) + " ms");
#endif
    }


private:

    bool findNext();

    /**
     * returns true if id is either in the map already or if there is still room for id, in the latter case the pair id, node_nr is inserted into the map
     * @param id string of the element to be mapped to an int value
     * @return true if id is either in the map already or if there is still room for id
     */
    inline bool canInsert(const std::string &id) {
        if (maxNumV > 0 && ipToNode.size() >= (unsigned long) maxNumV) {
            if (ipToNode.find(id) == ipToNode.end()) {
                return false;
            }
        }
        if (ipToNode.find(id) == ipToNode.end()) {
            ipToNode.emplace(id, nodeNr++);
        }
        return true;
    }

    mongocxx::instance instance;
    mongocxx::client client;
    std::unique_ptr<mongocxx::client_session> session;
    mongocxx::collection coll;
    mongocxx::options::find opts;
    std::unique_ptr<mongocxx::cursor> cursor;

    std::string srcName;
    std::string dstName;
    std::chrono::milliseconds timePassed;
    std::unordered_map<std::string, unsigned int> ipToNode;
    unsigned int nodeNr;
    int batchSize;
    int batchNr;
    int nrDocsExamined;
    unsigned int nrReturned;
    unsigned int nrSkipped;
    int maxNumV;
    bool nextFound;
    decomposition_tree::Edge next;
};


#endif //DBGTTHESIS_MONGODBDATAEXPERIMENTAL_H
