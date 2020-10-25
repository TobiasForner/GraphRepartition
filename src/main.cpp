//
// Created by tobias on 21.10.19.
//

#ifdef LOGGING_ENABLED

#include "spdlog/spdlog.h"
#include <spdlog/sinks/stdout_sinks.h>

#endif

#include "DecompositionTree.h"
#include "ComponentDecomposition.h"
#include "data_sources/MongoDBData.h"

#ifdef METIS_ENABLED

#include "ParMetisConnectedComponentComputation.h"

#endif

#include "data_structures/Graph.h"
#include "basic-profiling/IterationTimeWriter.h"
#include "ConfigReader.h"
#include "data_sources/MongoDBDataExperimental.h"
#include "check_cluster_mapping.h"
#include "data_sources/CSVDataSource.h"
#include "output/CLSWriter.h"
#include "dynamicLoop.h"
#include "staticLoop.h"

int main(int argc, char **argv) {
#ifdef LOGGING_ENABLED
    spdlog::set_level(spdlog::level::info);
#endif
    if (argc < 2) {
        std::cout << "ERROR: Not enough arguments provided to decomposition-tree!" << std::endl;
        std::cout << "Usage: ./decomposition-tree <path-to-config-file>" << std::endl;
        exit(1);
    }

    ConfigReader input = ConfigReader();
    input.readConfigOrConfigList(argv[1]);
    int config_nr = 1;
#ifdef LOGGING_ENABLED
    int debug_level = input.getIntVal("log-level");
    switch (debug_level) {
        case 0:
            spdlog::set_level(spdlog::level::off);
            break;
        case 1:
            spdlog::set_level(spdlog::level::err);
            break;
        case 2:
            spdlog::set_level(spdlog::level::info);
            break;
        case 3:
            spdlog::set_level(spdlog::level::debug);
            break;
        default:
            spdlog::set_level(spdlog::level::info);
    }
#endif
    auto timeAverages = std::vector<float>();
    auto commCostAverages = std::vector<float>();
    auto migCostAverages = std::vector<float>();


    while (true) {
        std::cout << "Config " + std::to_string(config_nr) + "\n";
        int alpha = input.getIntVal("alpha");
        std::string method = input.getVal("method");

        int nrRuns = input.getIntVal("nrRuns");
        auto runMilliseconds = std::vector<unsigned int>(nrRuns, 0);
        unsigned int timeForAllRuns = 0;
        auto communicationCostPerRun = std::vector<int>(nrRuns, 0);
        auto migrationCostPerRun = std::vector<int>(nrRuns, 0);

        int output_frequency = 10000;
        try {
            output_frequency = input.getIntVal("outputFrequency");
            std::cout << "Output frequency set to " + std::to_string(output_frequency) + "." << std::endl;
            //spdlog::info("Output frequency set to " + std::to_string(output_frequency) + ".");
        } catch (std::exception &e) {
            std::cout << "Output frequency defaulted to 10000." << std::endl;
            //spdlog::info("Output frequency defaulted to 10000.");
        }

        std::unique_ptr<CLSWriter> writer;
        try {
            writer = std::make_unique<CLSWriter>(input.getVal("summaryFile"));
            //general input data
            writer->writeToFile("alpha," + std::to_string(alpha));
            writer->writeToFile("augmentation," + std::to_string(input.getDoubleVal("augmentation")));
            writer->writeToFile("clusterNr," + std::to_string(input.getIntVal("clusterNr")));
            writer->writeToFile("clusterSize," + std::to_string(input.getIntVal("clusterSize")));
            if (input.getVal("dataSource") == "CSVData") {
                writer->writeToFile("inputFile," + input.getVal("input-file"));
            } else {
                writer->writeToFile("database," + input.getVal("database"));
                writer->writeToFile("collection," + input.getVal("collection"));
            }
        } catch (std::exception &e) {
            std::cout << "No summary file specified!" << std::endl;
            //spdlog::info("No summary file specified!");
        }

        for (int runNr = 0; runNr < nrRuns; runNr++) {
            std::unique_ptr<DecompositionMethod> decompositionMethod;
            std::unique_ptr<DataSource> data;
            std::cout << "Run " + std::to_string(runNr) << std::endl;
            bool dynamic = true;
            //initialize run structures
            if (method == "decompTree") {
                decomposition_tree::DecompositionTree::DELETION_METHOD deletionMethod = decomposition_tree::DecompositionTree::OLD;
                try {
                    auto delMethodString = input.getVal("deletionMethod");
                    if (delMethodString == "OLD") {
                        writer->writeToFile("deletionMethod,OLD");
                        deletionMethod = decomposition_tree::DecompositionTree::OLD;
                        std::cout << "Deletion method set to OLD." << std::endl;
                        //spdlog::info("Deletion method set to OLD.");
                    } else if (delMethodString == "CORE_ONLY_FILTER") {
                        writer->writeToFile("deletionMethod=,CORE_ONLY_FILTER");
                        deletionMethod = decomposition_tree::DecompositionTree::CORE_ONLY_FILTER;
                        std::cout << std::endl;
                        //spdlog::info("Deletion method set to CORE_ONLY_FILTER.");
                    } else if (delMethodString == "CORE_HALO_FILTER") {
                        writer->writeToFile("deletionMethod,CORE_HALO_FILTER");
                        deletionMethod = decomposition_tree::DecompositionTree::CORE_HALO_FILTER;
                        std::cout << "deletionMethod set to CORE_HALO_FILTER" << std::endl;
                        //spdlog::info("Deletion method set to CORE_HALO_FILTER.");
                    } else {
                        std::cout << "Invalid state in deletion method specification reached. Shutting down."
                                  << std::endl;
                        //spdlog::error("Invalid state in deletion method specification reached. Shutting down.");
                        exit(1);
                    }

                } catch (std::exception &e) {
                    writer->writeToFile("deletionMethod,OLD");
                    //spdlog::info("Deletion method defaulted to OLD.");
                }
                bool insertBetweenServers = true;
                try {
                    writer->writeToFile("insertBetweenServers," + std::to_string(insertBetweenServers));
                    insertBetweenServers = input.getBoolVal("insertBetweenServers");
                    //spdlog::info("insertBetweenServers set to " + std::to_string(insertBetweenServers));
                } catch (std::exception &e) {
                    writer->writeToFile("insertBetweenServers,1");
                    //spdlog::info("insertBetweenServers defaulted to true");
                }

                auto compGraphConstruction = decomposition_tree::DecompositionTree::COMP_GRAPH_CONSTRUCTION::NAIVE;


                try {
                    auto compGraphName = input.getVal("compGraphConstruction");
                    if (compGraphName == "NAIVE") {
                    } else if (compGraphName == "ALPHA_COMP") {
                        compGraphConstruction = decomposition_tree::DecompositionTree::ALPHA_COMP;
                    } else if (compGraphName == "CHILDREN") {
                        compGraphConstruction = decomposition_tree::DecompositionTree::CHILDREN;
                    }
                    //spdlog::info("compGraphConstruction set to " + compGraphName + ".");
                    writer->writeToFile("compGraphConstruction," + compGraphName);

                } catch (std::exception &e) {
                    writer->writeToFile("compGraphConstruction,NAIVE");
                    //spdlog::info("compGraphConstruction defaulted to NAIVE");
                }
                double augmentation = input.getDoubleVal("augmentation");
                if (augmentation <= 2) {
                    throw std::invalid_argument("Augmentation must be strictly larger than 2.");
                }

                decompositionMethod = std::make_unique<decomposition_tree::DecompositionTree>(
                        (unsigned int) input.getIntVal("maxNumV"), (unsigned int) alpha,
                        augmentation, input.getIntVal("clusterSize"),
                        (unsigned int) input.getIntVal("clusterNr"), input.getBoolVal("initRandomly"),
                        input.getBoolVal("tryOpt"), deletionMethod, compGraphConstruction, insertBetweenServers);

            } else if (method == "compDecomp") {
                double augmentation = input.getDoubleVal("augmentation");
                if (augmentation <= 2) {
                    throw std::invalid_argument("Augmentation must be strictly larger than 2.");
                }
                decompositionMethod = std::make_unique<decomposition_tree::ComponentDecomposition>(
                        (unsigned int) input.getIntVal("maxNumV"), (unsigned int) alpha,
                        augmentation, input.getIntVal("clusterSize"),
                        (unsigned int) input.getIntVal("clusterNr"), input.getBoolVal("initRandomly"));
            }
#ifdef METIS_ENABLED
            else if (method == "parmetis") {
                bool initMPI = runNr == 0;
                decomposition_tree::Graph g = decomposition_tree::Graph((unsigned int) input.getIntVal("maxNumV"));
                decompositionMethod = std::make_unique<ParMetisConnectedComponentComputation>(g, alpha,
                                                                                              input.getIntVal(
                                                                                                      "clusterNr"),
                                                                                              input.getIntVal(
                                                                                                      "clusterSize"),
                                                                                              static_cast<float>(input.getDoubleVal(
                                                                                                      "augmentation")),
                                                                                              input.getBoolVal(
                                                                                                      "initRandomly"),
                                                                                              initMPI);
            } else if (method == "metisStatic") {
                dynamic = false;
            }
#endif
            else {
                std::cout
                        << "No valid decomposition method specified in config. Please set method=<your-method> in the config file."
                        << std::endl;
                exit(1);
            }

            //spdlog::info("Connecting to data source...");

            if (input.getVal("dataSource") == "MongoDBData") {
                data = std::make_unique<MongoDBData>(input.getVal("uri"), input.getVal("database"),
                                                     input.getVal("collection"),
                                                     (unsigned int) input.getIntVal("batch-size"),
                                                     input.getIntVal("maxNumV"), input.getBoolVal("createIndex"),
                                                     input.getVal("sort-by"),
                                                     input.getVal("srcName"), input.getVal("dstName"));
            } else if (input.getVal("dataSource") == "MongoDBExperimental") {
                //spdlog::info("MongoDBExperimental");
                data = std::make_unique<MongoDBDataExperimental>(input.getVal("uri"), input.getVal("database"),
                                                                 input.getVal("collection"),
                                                                 input.getIntVal("batch-size"),
                                                                 input.getIntVal("maxNumV"), input.getVal("sort-by"),
                                                                 input.getVal("srcName"), input.getVal("dstName"));
            } else if (input.getVal("dataSource") == "CSVData") {
                data = std::make_unique<CSVDataSource>(input.getVal("input-file"));
            } else {
                std::cerr << "No valid data source specified!" << std::endl;
                exit(1);
            }


            //IterationTimeWriter iterationTimeWriter = IterationTimeWriter(input.getVal("timerFile"),
            //                                                              input.getIntVal("interval-size"));
            //spdlog::info("Connection to data source established and structures initialized");

            unsigned int invalid_matching_count = 0;

            //iterationTimeWriter.addConfig(configPairs);

            int migrationNr = 0;
            int communicationNr = 0;

            //perform run
            if (dynamic) {
                bool gatherStepStats = false;
                try {
                    gatherStepStats = input.getBoolVal("gatherStepStats");
                } catch (std::exception &e) {
                    gatherStepStats = false;
                }
                std::string stepStatsFilePrefix;
                if (gatherStepStats) {
                    stepStatsFilePrefix = input.getVal("stepStatsFilePrefix");
                    stepStatsFilePrefix += "run_" + std::to_string(runNr) + ".txt";
                }

                performDynamicRun(data.get(), decompositionMethod.get(), migrationNr, communicationNr, output_frequency,
                                  invalid_matching_count, input.getIntVal("clusterNr"), input.getIntVal("clusterSize"),
                                  input.getDoubleVal("augmentation"), gatherStepStats, stepStatsFilePrefix);
                runMilliseconds[runNr] = decompositionMethod->getMilliseconds();
                timeForAllRuns += decompositionMethod->getMilliseconds();
            } else if (!dynamic) {
#ifdef METIS_ENABLED
                unsigned int timePassed;
                performStaticRun(data.get(), input.getIntVal("maxNumV"), migrationNr,
                                 communicationNr, (unsigned int) input.getIntVal("clusterSize"),
                                 (unsigned int) input.getIntVal("clusterNr"), input.getBoolVal(
                                "initRandomly"), timePassed, invalid_matching_count);
                runMilliseconds[runNr] = timePassed;
                timeForAllRuns += timePassed;
#endif
            } else {
                std::cerr << "No valid decomposition method specified!";
                exit(1);
            }

            communicationCostPerRun[runNr] = communicationNr;
            migrationCostPerRun[runNr] = alpha * migrationNr;

#ifdef LOGGING_ENABLED
            spdlog::info("Run " + std::to_string(runNr) + " results:");
        spdlog::info("time: " + std::to_string(runMilliseconds[runNr]));
        spdlog::info("total cost: " + std::to_string(communicationCostPerRun[runNr] + migrationCostPerRun[runNr]));
        spdlog::info("communication cost: " + std::to_string(communicationCostPerRun[runNr]));
        spdlog::info("migration cost: " + std::to_string(migrationCostPerRun[runNr]));
#endif

            writer->writeToFile("\nRun " + std::to_string(runNr));
            writer->writeToFile("time," + std::to_string(runMilliseconds[runNr]));
            writer->writeToFile(
                    "total cost," + std::to_string(communicationCostPerRun[runNr] + migrationCostPerRun[runNr]));
            writer->writeToFile("communication cost," + std::to_string(communicationCostPerRun[runNr]));
            writer->writeToFile("migration cost," + std::to_string(migrationCostPerRun[runNr]));
#ifdef USE_DEBUG
            std::cout << "invalid matchings in run " + std::to_string(runNr) + ": " +
                         std::to_string(invalid_matching_count)
                      << std::endl;
            //spdlog::info("invalid matchings in run " + std::to_string(runNr) + ": " + std::to_string(invalid_matching_count));
#endif
        }


        if (writer) {
            //average data

            float totalTimeAvg = (1.0 * timeForAllRuns) / (1.0 * nrRuns);
            float totalCommunicationCost = 0;
            float totalMigrationCost = 0;


            for (unsigned int i = 0; i < nrRuns; i++) {
                totalCommunicationCost += 1.0 * communicationCostPerRun[i];
                totalMigrationCost += 1.0 * migrationCostPerRun[i];
            }

            timeAverages.emplace_back(totalTimeAvg);
            commCostAverages.emplace_back((1.0 * totalCommunicationCost) / (1.0 * nrRuns));
            migCostAverages.emplace_back((1.0 * totalMigrationCost) / (1.0 * nrRuns));

            writer->writeToFile("\nAverage data");
            writer->writeToFile("avg time," + std::to_string(totalTimeAvg));
            writer->writeToFile(
                    "avg total cost," +
                    std::to_string((1.0 * (totalCommunicationCost + totalMigrationCost)) / (1.0 * nrRuns)));
            writer->writeToFile(
                    "avg communication cost," + std::to_string((1.0 * totalCommunicationCost) / (1.0 * nrRuns)));
            writer->writeToFile("avg migration cost," + std::to_string((1.0 * totalMigrationCost) / (1.0 * nrRuns)));

            writer->closeFile();
        }

        std::cout << "Performed " + std::to_string(nrRuns) + " runs." << std::endl;
        if (!input.setToNextConfig()) {
            std::cout << "Shutting down." << std::endl;
            break;
        }
        config_nr += 1;
    }
    //write average data in file for boxplots etc
    std::unique_ptr<CLSWriter> writer = std::make_unique<CLSWriter>("summaryForBoxplots.txt");
    std::string s;
    for (unsigned int i = 0; i < timeAverages.size(); i++) {
        if (i > 0) {
            s += " & ";
        }
        s.append(std::to_string(timeAverages[i]));
    }
    s += "\\\\";
    writer->writeToFile("time averages");
    writer->writeToFile(s);

    s = "";
    for (unsigned int i = 0; i < commCostAverages.size(); i++) {
        if (i > 0) {
            s += " & ";
        }
        s.append(std::to_string(commCostAverages[i]));
    }
    s += "\\\\";
    writer->writeToFile("communication cost averages");
    writer->writeToFile(s);

    s = "";
    for (unsigned int i = 0; i < migCostAverages.size(); i++) {
        if (i > 0) {
            s += " & ";
        }
        s.append(std::to_string(migCostAverages[i]));
    }
    s += "\\\\";
    writer->writeToFile("migration cost averages");
    writer->writeToFile(s);

    s = "";
    for (unsigned int i = 0; i < migCostAverages.size(); i++) {
        if (i > 0) {
            s += " & ";
        }
        s.append(std::to_string(migCostAverages[i] + commCostAverages[i]));
    }
    s += "\\\\";
    writer->writeToFile("total cost averages");
    writer->writeToFile(s);
    writer->closeFile();


    return 0;
}