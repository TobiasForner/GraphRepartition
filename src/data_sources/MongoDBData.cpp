//
// Created by tobias on 10.11.19.
//

#include "MongoDBData.h"

MongoDBData::MongoDBData(const std::string &uri_string, const std::string &dbName, const std::string &collName,
                         unsigned int batch_size, int maxNumV, bool createIndex, const std::string &sortBy,
                         std::string srcName, std::string dstName)
        : uri(mongocxx::uri(uri_string)), client(mongocxx::client(uri)),
          database(client[dbName]),
          collection(database[collName]),
          cursor(collection.find({})), batchSize(batch_size), maxNumV(maxNumV), srcName(std::move(srcName)),
          dstName(std::move(dstName)) {
    //spdlog::info("[MongoDBData] uri set to " + uri_string + "; database = " + dbName + "; collection = " + collName);
    nrSkipped = 0;
    nrReturned = 0;
    pos = 0;
    node_nr = 0;
    batchNr = 0;
    auto start = std::chrono::high_resolution_clock::now();
    if (createIndex) {
        auto index_specification =
                bsoncxx::builder::stream::document{} << sortBy << 1 << bsoncxx::builder::stream::finalize;
        collection.create_index(std::move(index_specification));
    }

    //create options to sort by timestamp
    auto order = bsoncxx::builder::stream::document{} << "timestamp" << 1 << bsoncxx::builder::stream::finalize;
    opts = mongocxx::options::find{};
    opts.sort(order.view());
    try {
        cursor = collection.find({}, opts);
        fillBatch();
    } catch (mongocxx::exception &e) {
        std::cerr<<"Connection to MongoDB failed: \n" + (std::string) e.what()<<std::endl;
        exit(1);
    }
    auto end = std::chrono::high_resolution_clock::now();
    timePassed += std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
}

void MongoDBData::fillBatch() {
    unsigned int count = 0;
    batch = std::vector<decomposition_tree::Edge>();
    for (auto doc: cursor) {
        std::string src = doc["srcip"].get_utf8().value.to_string();
        std::string dst = doc["dstip"].get_utf8().value.to_string();

        //check if there is a maximum number of nodes set and whether the next edge would increase the node number
        if (maxNumV > 0 && ipToNode.size() >= (unsigned long) maxNumV) {
            if (ipToNode.find(src) == ipToNode.end() || ipToNode.find(dst) == ipToNode.end()) {
                nrSkipped++;
                continue;
            }
        }

        if (canInsertIntoBatch(src) && canInsertIntoBatch(dst)) {
            batch.emplace_back(decomposition_tree::Edge(ipToNode.find(src)->second, ipToNode.find(dst)->second, 1));
            count++;
        } else {
            continue;
        }

        if (count == batchSize) {
            break;
        }
    }
    batchNr++;
    pos = 0;
    if (cursor.begin() == cursor.end()) {
        //spdlog::debug("DB Cursor arrived at the end of the query!");
    } else {
        ++cursor.begin();
    }
}