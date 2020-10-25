//
// Created by Tobias on 16/11/2019.
//

#ifdef METIS_ENABLED

#pragma once

#include "../lib/ParMETIS-4.0.3/include/parmetis.h"
#include "../lib/ParMETIS-4.0.3/metis/include/metis.h"
#include "data_structures/Graph.h"
#include "/usr/include/mpich/mpi.h"
#include <algorithm>

#ifdef LOGGING_ENABLED
#include "spdlog/spdlog.h"
#endif

#include "DecompositionMethod.h"

#include <random>

#ifndef DBGTTHESIS_PARMETISCONNECTEDCOMPONENTCOMPUTATION_H
#define DBGTTHESIS_PARMETISCONNECTEDCOMPONENTCOMPUTATION_H

struct metis_graph {
    //idx_t *vtxdist;
    idx_t *xadj;
    idx_t *adjncy;
    //idx_t *vwgt;
    idx_t *adjwgt;
    idx_t *vsize;
};


/**
 * class for computing a partitioning using ParMETIS_V3_AdaptiveRepart and maintaining the needed data structures
 */
class ParMetisConnectedComponentComputation : public DecompositionMethod {
public:
    /**
     * Constructor
     * @param g graph representing the communication
     * @param alpha alpha, cost of migrations
     * @param clusterNr number of clusters
     * @param clusterSize size of each cluster
     * @param augmentation augmentation of the cluster size
     * @param initRandomly specifies whether the initial partition should be randomized
     * @param initMPI specifies whether MPI should be initialized, only set this to true once per execution
     */
    ParMetisConnectedComponentComputation(const decomposition_tree::Graph &g, int alpha, int clusterNr,
                                          unsigned int clusterSize, float augmentation, bool initRandomly,
                                          bool initMPI);

    static metis_graph convertToMetisGraph(const decomposition_tree::Graph &g) {
        auto xadj = new idx_t[g.getNodesNr() + 1];
        xadj[0] = 0;
        for (unsigned int i = 1; i < g.getNodesNr() + 1; i++) {
            xadj[i] = xadj[i - 1] + (idx_t) g.getEdges(i - 1).size();
        }

        unsigned int pos = 0;
        auto adjncy = new idx_t[g.getEdgesNr()];
        auto adjwgt = new idx_t[g.getEdgesNr()];
        for (unsigned int i = 0; i < g.getNodesNr(); i++) {
            for (auto e: g.getEdges(i)) {
                adjncy[pos] = (idx_t) e->getEnd();
                adjwgt[pos++] = (idx_t) e->getWeight();
            }
        }
        metis_graph metisGraph = {xadj, adjncy, adjwgt, nullptr};
        return metisGraph;
    }

    /**
     * adds an edge to the internal graph
     * @param a first node of the edge
     * @param b second node of the edge
     */
    inline void addEdge(decomposition_tree::node_ a, decomposition_tree::node_ b) override {
        auto e = decomposition_tree::Edge(a, b, 1);
        g.addEdge(e);
        updateInputs();
    }

    /**
     * updates the decomposition using parmetis
     * @param a first node of the edge that was inserted last
     * @param b second node of the edge that was inserted last
     */
    void updateDecomposition(decomposition_tree::node_ a, decomposition_tree::node_ b) override;

    /**
     * combines addEdge and updateDecomposition
     * @param a first node of the edge to be inserted
     * @param b second node of the edge to be inserted
     */
    inline void insertAndUpdate(decomposition_tree::node_ a, decomposition_tree::node_ b,
                                decomposition_tree::STEP_ACTION &stepAction, long &ms) override {
        ms = 0;
        addEdge(a, b);
        updateDecomposition(a, b);
        stepAction = decomposition_tree::NONE;
    }


    /**
     * returns the mapping as currently maintained by parmetis
     * @return current mapping
     */
    inline std::vector<unsigned int> getMapping() const override {
        std::vector<unsigned int> result = std::vector<unsigned int>();
        for (unsigned int i = 0; i < g.getNodesNr(); i++) {
            result.emplace_back(part[i]);
        }
        return result;
    }


    /**
     * Prints general run info, e.g. communication and migration costs
     */
    inline void printInfo() const override {
#ifdef LOGGING_ENABLED
        spdlog::info("ParMetis updateDecomposition time: " +
                     std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(totalTimePassed).count()));
#endif
    }


    /**
     * returns the time it took for parmetis to recompute the partitioning in ms
     * @return time in ms
     */
    inline long getMilliseconds() const override {
        return std::chrono::duration_cast<std::chrono::milliseconds>(totalTimePassed).count();
    }

private:
    void updateInputs();

private:
    decomposition_tree::Graph g;
    float augmentation;
    int alpha;
    int clusterNr;
    idx_t *vtxdist;
    idx_t *vwgt;
    idx_t *wgtflag;
    idx_t *numflag;
    idx_t *ncon;
    int *nparts;
    real_t *tpwgts;
    real_t *ubvec;
    //real_t *itr;
    idx_t *options;
    idx_t *edgecut;
    idx_t *part;
    MPI_Comm *comm;
};


#endif //DBGTTHESIS_PARMETISCONNECTEDCOMPONENTCOMPUTATION_H

#endif
