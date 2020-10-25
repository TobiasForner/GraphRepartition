//
// Created by tobias on 03.01.20.
//

#pragma once

#include "typedefs.h"

#include <vector>
#include <iostream>

#ifndef DBGTTHESIS_CHECK_CLUSTER_MAPPING_H
#define DBGTTHESIS_CHECK_CLUSTER_MAPPING_H

namespace decomposition_tree {
    /**
     * checks whether a given matching is valid
     * @param mapping vector representing the current matching, the ith entry represents the number of the cluster node i is matched to
     * @param nrClusters the total number of clusters
     * @param maxCapacity the capacity of each cluster
     * @return true if the matching is valid, false otherwise
     */
    inline bool
    check_mapping(const std::vector<unsigned int> mapping, const unsigned int nrClusters, const double maxCapacity) {
        std::vector<double> loads = std::vector<double>(nrClusters, 0);
        bool allValidlyMatched = true;
        for (auto entry:mapping) {
            if (entry >= nrClusters) {
                allValidlyMatched = false;
            }
            loads[entry]++;
        }
        if (!allValidlyMatched) {
            std::cout << "At least one matching is invalid, do more tests!";
        }
        bool allCapacitiesValid = true;
        for (unsigned int i = 0; i < loads.size(); i++) {
            if (loads[i] > maxCapacity) {
                allCapacitiesValid = false;
                std::cout << "Server " + std::to_string(i) + " is over capacity: Load is " + std::to_string(loads[i])
                          << "!\n";
            }
        }
        return allValidlyMatched && allCapacitiesValid;
    }

}

#endif //DBGTTHESIS_CHECK_CLUSTER_MAPPING_H
