//
// Created by Tobias on 16/11/2019.
//

#ifdef METIS_ENABLED

#include "ParMetisConnectedComponentComputation.h"

ParMetisConnectedComponentComputation::ParMetisConnectedComponentComputation(const decomposition_tree::Graph &g,
                                                                             const int alpha,
                                                                             const int clusterNr,
                                                                             const unsigned int clusterSize,
                                                                             const float augmentation,
                                                                             const bool initRandomly,
                                                                             const bool initMPI)
        : g(g), alpha(alpha) {
    this->augmentation = augmentation;
    this->clusterNr = clusterNr;
    totalTimePassed = std::chrono::microseconds(0);
    if (initMPI) {
        MPI_Init(nullptr, nullptr);
    }
    updateInputs();
    part = new idx_t[g.getNodesNr()];
    vwgt = nullptr;
    unsigned int nrNodesInCurrentPart = 0;
    idx_t currentPart = 0;
    for (unsigned int i = 0; i < g.getNodesNr(); i++) {
        part[i] = currentPart;
        nrNodesInCurrentPart++;
        if (nrNodesInCurrentPart == clusterSize) {
            currentPart++;
            nrNodesInCurrentPart = 0;
        }
    }
    if (initRandomly) {
        std::random_device rng;
        std::shuffle(&part[0], &part[g.getNodesNr() - 1], std::default_random_engine(rng()));
    }
}


void ParMetisConnectedComponentComputation::updateDecomposition(const decomposition_tree::node_ a,
                                                                const decomposition_tree::node_ b) {
    metis_graph metisGraph = convertToMetisGraph(g);

    vtxdist = new idx_t[2];
    vtxdist[0] = 0;
    vtxdist[1] = (idx_t) g.getNodesNr();
    auto start = std::chrono::high_resolution_clock::now();
    float itr = static_cast<float>(1.0 / (1.0 * alpha));
    int result = ParMETIS_V3_AdaptiveRepart(vtxdist, metisGraph.xadj, metisGraph.adjncy, vwgt, metisGraph.vsize,
                                            metisGraph.adjwgt, wgtflag, numflag, ncon, nparts,
                                            tpwgts, ubvec, &itr, options, edgecut, part, comm);
    auto end = std::chrono::high_resolution_clock::now();
    totalTimePassed += std::chrono::duration_cast<std::chrono::microseconds>(end - start);

#ifdef LOGGING_ENABLED

    spdlog::debug(
            "[ParMetisConnectedComponentComputation] Performing ParMETIS_V3_AdaptiveRepart following insertion between " +
            std::to_string(a) + " and " + std::to_string(b));
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


    delete[]vtxdist;
    delete[]metisGraph.xadj;
    delete[]metisGraph.adjncy;
    //vwgt: nullptr
    //vsize is nullptr currently
    delete[]metisGraph.adjwgt;
    delete wgtflag;
    delete numflag;
    delete ncon;
    delete nparts;
    delete[] tpwgts;
    delete[] ubvec;
    //delete itr;
    delete[] options;
    delete edgecut;
    delete comm;
}


void ParMetisConnectedComponentComputation::updateInputs() {
    vtxdist = new idx_t[2];
    vtxdist[0] = 0;
    vtxdist[1] = (idx_t) g.getNodesNr();
    wgtflag = new idx_t;
    *wgtflag = 1;
    numflag = new idx_t;
    *numflag = 0;
    ncon = new idx_t;
    *ncon = 1;
    nparts = new idx_t;
    *nparts = clusterNr;//adjust as necessary
    tpwgts = new real_t[static_cast<unsigned long>((*ncon) * (*nparts))];
    for (int i = 0; i < (*ncon) * (*nparts); i++) {
        tpwgts[i] = (real_t) 1 / (real_t) (*nparts);
    }
    ubvec = new real_t[(unsigned long) *ncon];
    ubvec[0] = augmentation;
    //itr is set  in the partitioning function itself
    options = new idx_t[1];
    options[0] = 0;
    edgecut = new idx_t;
    //part is initialized in the constructor and then adapted by parmetis, hence not configured here

    comm = new MPI_Comm;
    *comm = MPI_COMM_WORLD;
}

#endif