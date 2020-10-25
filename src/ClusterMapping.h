//
// Created by tobias on 17.12.19.
//
#include <algorithm>
#include <random>
#include <vector>
#include "typedefs.h"
#include "data_structures/UnionFind.h"

#ifndef DBGTTHESIS_CLUSTERMAPPING_H
#define DBGTTHESIS_CLUSTERMAPPING_H


class ClusterMapping {
public:
    /**
     * constructor
     * @param nrNodes nr of nodes that are to be mapped
     * @param clusterSize size of each cluster
     * @param clusterNr number of clusters
     * @param augmentation augmentation of the clusterSize
     * @param initRandomly specifies whether the initial mapping should be random
     */
    ClusterMapping(unsigned int nrNodes, unsigned int clusterSize, unsigned int clusterNr,
                   double augmentation, double epsilon,
                   bool initRandomly);


    /**
     * returns the current mapping
     * @return current mapping
     */
    std::vector<decomposition_tree::node_> getMapping() const;


    /**
     * updates the current mapping based on the current components and returns which components should be deleted
     * @param components current components
     * @param toDelete vector in which components are inserted that should be deleted
     */
    void updateMapping(const std::vector<std::vector<decomposition_tree::node_>> &components,
                       std::vector<std::vector<decomposition_tree::node_>> &toDelete, bool& migration);


    /**
     * Returns whether x and y are mapped to the same cluster
     * @param x first node
     * @param y second node
     * @return true if x and y are mapped to the same cluster, false otherwise
     */
    bool areMappedToSameCluster(decomposition_tree::node_ x, decomposition_tree::node_ y) const;


    /**
     * Returns whether x and y are in the same component
     * @param x first node
     * @param y second node
     * @return true if x and y are in the same component, false otherwise
     */
    bool areInSameComponent(decomposition_tree::node_ x, decomposition_tree::node_ y) const;


    /**
     * Returns the total number of migrtions
     * @return number of migrations
     */
    unsigned int getMigrationNr() const;

    /**
     * debug function that compares the internal structures (i.e. union find) with the components
     * @param comps components the internal structures are compared with
     * @return true if the internal structures are do not conflict with the components, false otherwise
     */
    bool internalStructuresSameAsComps(const std::vector<std::vector<decomposition_tree::node_>> &comps);


    /**
     * returns the cluster size
     * @return cluster size
     */
    unsigned int getClusterSize() const {
        return clusterSize;
    }


    /**
     * returns the number of clusters
     * @return number of clusters
     */
    unsigned int getClusterNr() const {
        return clusterNr;
    }


    /**
     * returns the augmentation
     * @return augmentation
     */
    double getAugmentation() const {
        return augmentation;
    }


private:
    UnionFind uf;
    unsigned int nrNodes;
    unsigned int clusterSize;
    unsigned int nrMoves;
    unsigned int clusterNr;
    double augmentation;
    double epsilon;
    //contains information about how much space is left in each cluster
    std::vector<double> clusterCapacities;
    //maps nodes(indices to clusters (values)
    std::vector<decomposition_tree::node_> nodesToClusters;
    //vector of current components, might not be really needed
    std::vector<std::vector<decomposition_tree::node_>> components;

    std::vector<unsigned int> reservations;


    /**
     * collocates the specified sets
     * @param setIds ids specifying the sets that are to be collocated
     * @param newComponents new components
     * @param sizeToMerge size if the resulting set
     */
    void collocateSets(const std::vector<unsigned int> &setIds, unsigned long sizeToMerge);


    /**
     * moves the sets whose ids are in setIds to the cluster denoted by clusterId and merges the sets to targetSet in
     * the union find structure
     * @param setIds ids of sets that are to be moved
     * @param clusterId id of the target cluster
     * @param targetSet set the other sets are to be merged with
     * @param useReservationOnly specifies whether the target cluster already contains some of the nodes
     */
    void moveSetsToCluster(const std::vector<unsigned int> &setIds, unsigned int clusterId, unsigned int targetSet, bool useReservationOnly);


    /**
     * moves the set represented by x in the union find data structure to clusterId
     * @param x id of the set that is to be moved
     * @param clusterId id of the target cluster
     * @param resUsed specifies whether the space is taken from the server capacity or from existing reservations
     */
    void moveToCluster(unsigned int x, unsigned int clusterId, bool resUsed);


    /**
     * computes the additional reservation for a set of size setSize
     * @param setSize size of the set
     * @return additional reservation
     */
    unsigned int computeReservation(unsigned long setSize) {
        auto setRes = static_cast<unsigned int>(std::floor(epsilon * (double) setSize));
        return setRes < clusterSize - setRes ? setRes : clusterSize - setRes;
    }

    void check_server_capacities();

};


#endif //DBGTTHESIS_CLUSTERMAPPING_H
