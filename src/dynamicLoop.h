//
// Created by tobias on 15.02.20.
//
#include "data_sources/DataSource.h"
#include "DecompositionMethod.h"
#include "output/CLSWriter.h"

#ifndef DBGTTHESIS_DYNAMICLOOP_H
#define DBGTTHESIS_DYNAMICLOOP_H

/**
 * main loop of a dynamic run
 * @param data data source
 * @param decompositionMethod decomposition method containing an algorithm for the repartitioning
 * @param migrationNr the number of migrations will be inserted here
 * @param communicationNr the number of communications between nodes mapped to different servers will be inserted here
 * @param output_frequency specifies how often information about the run should be printed, higher means less frequent
 * @param invalid_matching_count the number of times an invalid matching is encountered is inserted here, only if debug is enabled
 * @param clusterNr number of clusters
 * @param clusterSize size of each cluster
 * @param augmentation augmentation
 */
void
performDynamicRun(DataSource *data, DecompositionMethod *decompositionMethod, int &migrationNr, int &communicationNr,
                  const int output_frequency, unsigned int &invalid_matching_count, const unsigned int clusterNr,
                  const int clusterSize, const double augmentation, bool gatherStepStats = false,
                  const std::string &stepStatFile = "") {
    std::unique_ptr<CLSWriter> writer;
    if (gatherStepStats) {
        writer = std::make_unique<CLSWriter>(stepStatFile);
        writer->writeToFile("request_nr, action, time(ms), migration_nr, requests_paid_nr");
    }
    int count = 0;
    while (data->hasNext()) {
        if (count % output_frequency == 0) {
#ifdef USE_DEBUG
            if (!decomposition_tree::check_mapping(decompositionMethod->getMapping(),
                                                   clusterNr,
                                                   clusterSize * augmentation)) {
                invalid_matching_count++;
            }
#endif
            std::cout << "Iteration " + std::to_string(count) << std::endl;
            //data->printInfo();
#ifdef COLLECT_STATS
            decompositionMethod->printInfo();
#endif
        }
        count++;

        auto oldMapping = decompositionMethod->getMapping();

        auto e = data->getNext();
        decomposition_tree::STEP_ACTION stepAction = decomposition_tree::NONE;
        long ms = 0;
        decompositionMethod->insertAndUpdate(e.getStart(), e.getEnd(), stepAction, ms);

        auto newMapping = decompositionMethod->getMapping();

        int migration_nr = 0;
        int comm_requests_paid = 0;
        if (newMapping[e.getStart()] != newMapping[e.getEnd()]) {
            comm_requests_paid++;
            ++communicationNr;
        }
        for (unsigned int i = 0; i < oldMapping.size(); i++) {
            if (oldMapping[i] != newMapping[i]) {
                migration_nr++;
                migrationNr++;
            }
        }
        if (gatherStepStats) {
            std::string action_string;
            switch (stepAction) {
                case decomposition_tree::NONE:
                    action_string = "NONE";
                    break;
                case decomposition_tree::MIGRATION:
                    action_string = "MIGRATION";
                    break;
                case decomposition_tree::REQUEST:
                    action_string = "REQUEST";
                    break;
                case decomposition_tree::SKIPPED:
                    action_string = "SKIPPED";
                    break;
                case decomposition_tree::DELETION:
                    action_string = "DELETION";
                    break;
            }
            writer->writeToFile(std::to_string(count) + "," + action_string + "," + std::to_string(ms) + "," +
                                std::to_string(migration_nr) + "," + std::to_string(comm_requests_paid));
        }
    }
    if (gatherStepStats) {
        writer->closeFile();
    }
    decompositionMethod->printInfo();
    std::cout << "Iteration " + std::to_string(count) + " completed." << std::endl;
}

#endif //DBGTTHESIS_DYNAMICLOOP_H
