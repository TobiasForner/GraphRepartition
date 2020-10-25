//
// Created by tobias on 15.02.20.
//

#ifdef METIS_ENABLED

#include "../lib/ParMETIS-4.0.3/include/parmetis.h"
#include "../lib/ParMETIS-4.0.3/metis/include/metis.h"
#include "data_structures/Graph.h"
#include "ParMetisConnectedComponentComputation.h"

#ifndef DBGTTHESIS_METISSTATIC_H
#define DBGTTHESIS_METISSTATIC_H

/**
 * runs the static metis algorithm
 * @param g graph representing the communication requests
 * @param clusterNr number of clusters
 * @param initialMapping initial mapping of nodes to clusters
 * @param newMapping destination for the new mapping the algorithm comes up with
 * @param msPassed running time of the algorithm
 */
void
run_METIS_static(const decomposition_tree::Graph &g, const int clusterNr,
                 const std::vector<unsigned int> initialMapping,
                 std::vector<unsigned int> &newMapping, unsigned int &msPassed) {
    auto graph = ParMetisConnectedComponentComputation::convertToMetisGraph(g);

    idx_t nvtxs = (idx_t) initialMapping.size();
    idx_t ncon = 1;
    int nparts = clusterNr;

    auto part = new idx_t[initialMapping.size()];
    for (unsigned int i = 0; i < initialMapping.size(); i++) {
        part[i] = initialMapping[i];
    }

    idx_t *objval = new idx_t;//cut value, i.e. number of communication requests paid

    auto vwgt = new idx_t[ncon * nvtxs];
    for (unsigned int i = 0; i < ncon * nvtxs; i++) {
        vwgt[i] = 1;
    }

    auto tpwgts = new real_t[nparts * ncon];
    for (unsigned int i = 0; i < nparts * ncon; i++) {
        tpwgts[i] = 1.0 / (1.0 * nparts);
    }

    auto start = std::chrono::high_resolution_clock::now();
    int result = METIS_PartGraphRecursive(&nvtxs,
                                          &ncon,
                                          graph.xadj,
                                          graph.adjncy,
                                          vwgt,//vwgt
                                          NULL, //vsize
                                          graph.adjwgt,
                                          &nparts,
                                          tpwgts,//tpwgts
                                          NULL,//ubvec
                                          NULL,//options
                                          objval,
                                          part);

    auto end = std::chrono::high_resolution_clock::now();
    auto totalTimePassed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    msPassed = totalTimePassed.count();
#ifdef LOGGING_ENABLED

    spdlog::debug(
            "[ParMetisConnectedComponentComputation] Performing METIS_PartGraphRecursive");
    switch (result) {
        case METIS_OK:
            spdlog::debug("ParMETIS: OK");
            break;
        case METIS_ERROR:
            spdlog::error("ParMETIS: ERROR");
            break;
        default:
            spdlog::error("ParMETIS: UNKNOWN");
    }
#endif
    newMapping = std::vector<unsigned int>(initialMapping.size(), 0);
    for (unsigned int i = 0; i < initialMapping.size(); i++) {
        newMapping[i] = part[i];
    }
    delete[]vwgt;
    delete[]tpwgts;
    delete objval;
    delete graph.xadj;
    delete graph.adjwgt;
    delete graph.adjncy;
    delete[]part;
}

#endif //DBGTTHESIS_METISSTATIC_H

#endif
