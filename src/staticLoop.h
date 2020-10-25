//
// Created by tobias on 15.02.20.
//

#ifdef METIS_ENABLED

#ifndef DBGTTHESIS_STATICLOOP_H
#define DBGTTHESIS_STATICLOOP_H

#include "data_sources/DataSource.h"
#include "DecompositionMethod.h"
#include "data_structures/Graph.h"
#include "MetisStatic.h"
#include <vector>

/**
 * performs one run of the static algorithm
 * @param data data source
 * @param nrNodes number of different nodes
 * @param migrationNr destination for the number of migrations needed
 * @param communicationNr destination for the number of paid communication requests
 * @param clusterSize size of each cluster
 * @param clusterNr number of clusters
 * @param initRandomly specifies whether the initial partition should be generated randomly
 * @param msPassed time used by the algorithm
 * @param invalid_matching_count destination for the number of invalid matchings encountered during the run, for debug purposes
 */
inline void performStaticRun(DataSource *data, const unsigned int nrNodes, int &migrationNr,
                             int &communicationNr, const unsigned int clusterSize, const unsigned int clusterNr,
                             const bool initRandomly, unsigned int &msPassed, unsigned int &invalid_matching_count) {
    decomposition_tree::Graph g = decomposition_tree::Graph(nrNodes);
    int count = 0;
    while (data->hasNext()) {
        count++;
        auto e = data->getNext();
        g.addEdge(e);
    }
    std::cout << "Static run of " + std::to_string(count) + " iterations complete. Calculating cost." << std::endl;
    auto oldMapping = std::vector<unsigned int>();
    for (unsigned int cluster = 0; cluster < clusterNr; cluster++) {
        oldMapping.insert(oldMapping.end(), clusterSize, cluster);
    }
    if (initRandomly) {
        std::random_device rng;
        std::shuffle(oldMapping.begin(), oldMapping.end(), std::default_random_engine(rng()));
    }

    std::vector<unsigned int> newMapping;

    run_METIS_static(g, clusterNr, oldMapping, newMapping, msPassed);

#ifdef USE_DEBUG
    if (!decomposition_tree::check_mapping(newMapping,
                                           clusterNr,
                                           clusterSize)) {
        invalid_matching_count++;
    }
#endif
    for (unsigned int i = 0; i < newMapping.size(); i++) {
        if (newMapping[i] != oldMapping[i]) {
            migrationNr++;
        }
    }
#ifdef USE_DEBUG
    unsigned int edge_count = 0;
#endif
    for (unsigned int node = 0; node < newMapping.size(); node++) {
        auto nodeEdges = g.getEdges(node);
        for (auto e: nodeEdges) {
#ifdef USE_DEBUG
            edge_count += e->getWeight();
#endif
            if (newMapping[e->getStart()] != newMapping[e->getEnd()] && e->getStart() > e->getEnd()) {
                communicationNr += e->getWeight();
            }
        }
    }
#ifdef USE_DEBUG
//edges in graph are directed, hence each request is counted twice
    if (edge_count != 2 * count) {
        std::cerr << "Edges in graph are not of weight equal to the number of requests: " + std::to_string(edge_count) +
                     " are in graph and " + std::to_string(count) + " requests were recorded." << std::endl;
        exit(1);
    }
#endif
}

#endif //DBGTTHESIS_STATICLOOP_H

#endif
