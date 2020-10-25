//
// Created by tobias on 10.11.19.
//


#include <cstdint>
#include <iostream>
#include <vector>
#include <bsoncxx/v_noabi/bsoncxx/json.hpp>
#include <mongocxx/v_noabi/mongocxx/client.hpp>
#include <mongocxx/v_noabi/mongocxx/stdx.hpp>
#include <mongocxx/v_noabi/mongocxx/uri.hpp>
#include "mongocxx/v_noabi/mongocxx/instance.hpp"
#include "mongocxx/v_noabi/mongocxx/collection.hpp"
#include "mongocxx/v_noabi/mongocxx/exception/exception.hpp"
#include <bsoncxx/v_noabi/bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/v_noabi/bsoncxx/builder/stream/helpers.hpp>
#include <unordered_map>

#ifdef LOGGING_ENABLED
#include "spdlog/spdlog.h"
#endif


#include "../Edge.h"
#include "DataSource.h"


using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;


#ifndef DBGTTHESIS_MONGODBDATA_H
#define DBGTTHESIS_MONGODBDATA_H

/**
 * class defining a MongoDB data source
 */
class MongoDBData : public DataSource {
public:
    /**
     *
     * @param uri_string uri string for the MongoDB
     * @param dbName name of the database
     * @param collName name of the collection in the database
     * @param batch_size number of edges that is to be cached
     * @param maxNumV maximum number of vertices that is to be allowed; if set to a non-positive number this will be
     *        ignored, otherwise new edges are skipped if they contain a new endpoint
     * @param createIndex specifies whether an index supporting the required sorting should be created
     * @param sortBy name of the field to sort by, leave empty to skip sorting
     * @param srcName name of the source field in the database
     * @param dstName name of the destination field in the database
     */
    MongoDBData(const std::string &uri_string, const std::string &dbName, const std::string &collName,
                unsigned int batch_size, int maxNumV, bool createIndex, const std::string &sortBy,
                std::string srcName, std::string dstName);

    /**
     * Gives information about whether there is an additional edge in the database
     * @return true if there is an edge, false otherwise
     */
    inline bool hasNext() override {
        return pos < batch.size() || cursor.begin() != cursor.end();
    }

    /**
     * returns the next edge in the collection
     * @return the next edge in the collection
     */
    inline decomposition_tree::Edge getNext() override {
        auto start = std::chrono::high_resolution_clock::now();
        if (pos >= batchSize) {
            fillBatch();
        }
        nrReturned++;
        auto end = std::chrono::high_resolution_clock::now();
        timePassed += std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return batch[pos++];
    }

    /**
     * prints basic info about the current run
     */
    inline void printInfo() override {
#ifdef LOGGING_ENABLED
        spdlog::info("Total: " + std::to_string(nrSkipped + nrReturned) + ", Skipped: " + std::to_string(nrSkipped) +
                     ", Returned: " + std::to_string(nrReturned) + ", total DB time: " +
                     std::to_string(timePassed.count()) + " ms");
#endif

    }

private:
    /**
     * returns true if id is either in the map already or if there is still room for id, in the latter case the pair id, node_nr is inserted into the map
     * @param id string of the element to be mapped to an int value
     * @return true if id is either in the map already or if there is still room for id
     */
    inline bool canInsertIntoBatch(const std::string &id) {
        if (maxNumV > 0 && ipToNode.size() >= (unsigned long) maxNumV) {
            if (ipToNode.find(id) == ipToNode.end()) {
                return false;
            }
        }
        if (ipToNode.find(id) == ipToNode.end()) {
            ipToNode.emplace(id, node_nr++);
        }
        return true;
    }

    /**
     * fills the buffer with new edges from the collection if possible
     */
    void fillBatch();

    mongocxx::instance instance;
    mongocxx::uri uri;
    mongocxx::client client;
    mongocxx::database database;
    mongocxx::collection collection;
    mongocxx::cursor cursor;
    mongocxx::options::find opts;

    std::chrono::milliseconds timePassed{std::chrono::milliseconds(0)};
    //maps ips to nodes
    std::unordered_map<std::string, unsigned int> ipToNode;
    std::vector<decomposition_tree::Edge> batch;
    unsigned int node_nr;
    unsigned int pos;
    unsigned int batchSize;
    unsigned int batchNr;
    int maxNumV;
    //unsigned int docsLimit;

    std::string srcName;
    std::string dstName;

    unsigned int nrSkipped;
    unsigned int nrReturned;
};


#endif //DBGTTHESIS_MONGODBDATA_H
