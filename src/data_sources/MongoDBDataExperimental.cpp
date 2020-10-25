//
// Created by tobias on 10.12.19.
//

#include "MongoDBDataExperimental.h"

MongoDBDataExperimental::MongoDBDataExperimental(const std::string &uri, const std::string &db,
                                                 const std::string &collection, int batchSize,
                                                 int maxNumV, std::string sortBy, std::string srcName,
                                                 std::string dstName)
        : srcName(std::move(srcName)), dstName(std::move(dstName)), timePassed(std::chrono::milliseconds(0)),
          batchSize(batchSize),
          maxNumV(maxNumV),
          next(decomposition_tree::Edge(0, 0, 0)) {
    client = mongocxx::client(mongocxx::uri(uri));
    session = std::make_unique<mongocxx::client_session>(client.start_session());
    coll = client[db][collection];
    opts = mongocxx::options::find();
    if (!sortBy.empty()) {
        opts.sort(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp(sortBy, 1)));
    }

    opts.limit(batchSize);
    nrDocsExamined = 0;
    batchNr = 0;
    ipToNode = std::unordered_map<std::string, unsigned int>();
    nodeNr = 0;
    nrSkipped = 0;
    nrReturned = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();
    nextFound = findNext();
    std::chrono::time_point<std::chrono::high_resolution_clock> end = std::chrono::high_resolution_clock::now();
    timePassed += std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
}


MongoDBDataExperimental::~MongoDBDataExperimental() = default;


bool MongoDBDataExperimental::findNext() {
    if (nrDocsExamined % batchSize == 0) {
        opts.skip(batchNr * batchSize);
        cursor = std::make_unique<mongocxx::cursor>(coll.find({}, opts));
        batchNr++;
    } else {
        ++cursor->begin();
    }
    auto cursorPos = cursor->begin();
    if (cursorPos == cursor->end()) {
        return false;
    }
    ++nrDocsExamined;
    auto doc = *cursorPos;
    std::string src;
    std::string dst;
    if (doc[srcName].type() == bsoncxx::types::b_utf8::type_id) {
        src = doc[srcName].get_utf8().value.to_string();
        dst = doc[dstName].get_utf8().value.to_string();
    } else if (doc[srcName].type() == bsoncxx::types::b_int32::type_id) {
        src = std::to_string(doc[srcName].get_int32());
        dst = std::to_string(doc[dstName].get_int32());
    } else {
        throw std::runtime_error("Encountered type " + bsoncxx::to_string(doc[srcName].type()) +
                                 " as the source type, but it is not supported at the moment!");
    }


    if (canInsert(src) && canInsert(dst)) {
        //nrReturned++;
        next = decomposition_tree::Edge(ipToNode.find(src)->second, ipToNode.find(dst)->second, 1);
        return true;
    }
    nrSkipped++;
    return findNext();
}