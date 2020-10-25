//
// Created by tobias on 17.12.19.
//

#include <iostream>
#include "ClusterMapping.h"

ClusterMapping::ClusterMapping(const unsigned int nrNodes, const unsigned int clusterSize, const unsigned int clusterNr,
                               const double augmentation, const double epsilon, const bool initRandomly)
        : nrNodes(nrNodes), clusterSize(clusterSize), clusterNr(clusterNr),
          augmentation(augmentation), epsilon(epsilon) {
    reservations = std::vector<unsigned int>(nrNodes, 0);
    nrMoves = 0;
    uf = UnionFind();
    for (unsigned int i = 0; i < nrNodes; i++) {
        uf.makeSet(i);
    }
    nodesToClusters = std::vector<unsigned int>();
    for (unsigned int cluster = 0; cluster < clusterNr; cluster++) {
        nodesToClusters.insert(nodesToClusters.end(), clusterSize, cluster);
    }
    if (initRandomly) {
        std::random_device rng;
        std::shuffle(nodesToClusters.begin(), nodesToClusters.end(), std::default_random_engine(rng()));
    }
    for (unsigned int i = 0; i < clusterNr; i++) {
        clusterCapacities.emplace_back((2 + epsilon) * clusterSize);
    }
    for (auto nTC: nodesToClusters) {
        clusterCapacities[nTC]--;
    }
    components = std::vector<std::vector<decomposition_tree::node_ >>();
    for (decomposition_tree::node_ i = 0; i < nrNodes; i++) {
        components.emplace_back(std::vector<decomposition_tree::node_>(1, i));
    }
}


std::vector<decomposition_tree::node_> ClusterMapping::getMapping() const {
    return nodesToClusters;
}


void ClusterMapping::updateMapping(const std::vector<std::vector<decomposition_tree::node_>> &newComponents,
                                   std::vector<std::vector<decomposition_tree::node_>> &toDelete, bool &migration) {
#ifdef USE_DEBUG
    check_server_capacities();
    internalStructuresSameAsComps(components);
#endif
    if (newComponents.size() >= components.size()) {
//by insertions the number of alpha connected components can only go down
        migration = false;
        return;
    }
    auto internalComponents = std::vector<std::vector<decomposition_tree::node_>>();
    std::vector<unsigned int> idOfSetsToMerge;
    unsigned long sizeToMerge = 0;

    bool del = false;
    unsigned count = 0;
    for (auto component: newComponents) {
        if (component.size() > clusterSize) {
            del = true;
            toDelete.emplace_back(component);
            for (auto n:component) {
                internalComponents.emplace_back(std::vector<decomposition_tree::node_>(1, n));
            }
        } else {
            if (uf.getSize(component[0]) < component.size()) {
#ifdef USE_DEBUG
                if (sizeToMerge != 0) {
                    throw std::logic_error("two components have changed.");
                }
#endif
                sizeToMerge = component.size();
//found component that is the result of the merge, now we need to compute which components were merged to result in the current component
                UnionFind tmp = UnionFind();
                for (unsigned int i = 0; i < component.size(); i++) {
                    if (tmp.makeSet(component[i])) {
                        idOfSetsToMerge.emplace_back(component[i]);
                        for (auto k:uf.getSetOf(component[i])) {
                            tmp.makeSet(k);
                            tmp.unionSet(component[i], k);
                        }
                    }
                }
            }
            internalComponents.emplace_back(component);
        }
        count++;
    }
    if (del) {
        components = internalComponents;
//split sets that are to be deleted in the union find
        for (auto &comp:toDelete) {
            for (auto node:comp) {
                clusterCapacities[nodesToClusters[node]] += reservations[uf.find(node)];
                reservations[uf.find(node)] = computeReservation(1);
                uf.splitSetOf(node);
                clusterCapacities[nodesToClusters[node]] -= reservations[uf.find(node)];
            }
        }
#ifdef USE_DEBUG
        check_server_capacities();
        internalStructuresSameAsComps(components);
#endif
        migration = false;
        return;
    }
    if (sizeToMerge == 0) {
//compare uf state with new components
        internalStructuresSameAsComps(newComponents);
        throw std::logic_error("Update without deletion and merge should not reach this point!");
    }
    migration = true;

    collocateSets(idOfSetsToMerge, sizeToMerge);
    components = newComponents;
#ifdef USE_DEBUG
    check_server_capacities();
    internalStructuresSameAsComps(components);
    internalStructuresSameAsComps(newComponents);
#endif
}


bool
ClusterMapping::areMappedToSameCluster(const decomposition_tree::node_ x, const decomposition_tree::node_ y) const {
    return nodesToClusters[x] == nodesToClusters[y];
}

bool ClusterMapping::areInSameComponent(const decomposition_tree::node_ x, const decomposition_tree::node_ y) const {
    return uf.find(x) == uf.find(y);
}


unsigned int ClusterMapping::getMigrationNr() const {
    return nrMoves;
}


bool ClusterMapping::internalStructuresSameAsComps(const std::vector<std::vector<decomposition_tree::node_>> &comps) {
    bool result = true;
    for (auto &comp: comps) {
        for (auto node:comp) {
            if (uf.getSize(node) != comp.size()) {
                std::cout << "component has different size than corresponding uf set" << std::endl;
                result = false;
            }
//check that uf set node belongs to is the same as comp
            auto ufComp = uf.getSetOf(node);
            for (auto n:ufComp) {
                bool nFound = false;
                for (auto compsNode:comp) {
                    if (compsNode == n) {
                        nFound = true;
                        break;
                    }
                }
                if (!nFound) {
                    std::cout << "Difference between UnionFind and comps found!" << std::endl;
                    result = false;
                }
            }
        }
    }
    return result;
}


void ClusterMapping::collocateSets(const std::vector<unsigned int> &setIds,
                                   unsigned long sizeToMerge) {
/*
//check if all the components can be moved to a cluster that already contains components
auto potentialTargets = std::vector<unsigned int>();
auto sampleNodeOfPotentialTarget = std::vector<unsigned int>();
//nr of nodes from the newly merged component that are already on the cluster and hence do not need to be moved
auto nrNodesToBeCollocatedInCluster = std::vector<unsigned int>();
//reservations in the potential target clusters of nodes that are to be collocated
auto reservationsThatCanBeUsedPerCluster = std::vector<unsigned int>();*/

    auto reserve = computeReservation(sizeToMerge);
    double totalSpaceNeeded = (double) sizeToMerge + reserve;

//check all servers and move to the one that has enough space and also contains the most amount of nodes that are to be collocated already
    auto nrNodesToBeCollocatedInCluster = std::vector<unsigned int>(clusterNr, 0);
    auto reservationsThatCanBeUsedPerCluster = std::vector<unsigned int>(clusterNr, 0);
    for (unsigned int i:setIds) {
        nrNodesToBeCollocatedInCluster[nodesToClusters[i]] += uf.getSize(i);
        reservationsThatCanBeUsedPerCluster[nodesToClusters[i]] += reservations[uf.find(i)];
    }

    unsigned int bestTarget = clusterNr;
    bool reservationsEnough = false;

    for (unsigned int currCluster = 0; currCluster < clusterNr; currCluster++) {
        /*
        if(reservationsThatCanBeUsedPerCluster[currCluster]>totalSpaceNeeded-reserve-nrNodesToBeCollocatedInCluster[currCluster]){
            reservationsEnough=true;
            bestTarget=currCluster;
            break;
        }*/
        if (clusterCapacities[currCluster] + nrNodesToBeCollocatedInCluster[currCluster] +
            reservationsThatCanBeUsedPerCluster[currCluster] >= totalSpaceNeeded) {
            if (bestTarget < clusterNr &&
                nrNodesToBeCollocatedInCluster[bestTarget] >= nrNodesToBeCollocatedInCluster[currCluster]) {
//best target is a at least as good as currCluster
                continue;
            }
            bestTarget = currCluster;
        }
    }
    if (bestTarget >= clusterNr) {
        throw std::logic_error("No suitable target cluster could be found!");
    }

    moveSetsToCluster(setIds, bestTarget, setIds[0], reservationsEnough);

/*
for (unsigned int i: setIds) {
    //check if cluster is already a potential target and increase the corresponding number if so
    bool found = false;
    for (unsigned int k = 0; k < potentialTargets.size(); k++) {
        if (potentialTargets[k] == nodesToClusters[i]) {
            nrNodesToBeCollocatedInCluster[k] += uf.getSize(i);
            reservationsThatCanBeUsedPerCluster[k] += reservations[i];
            found = true;
            break;
        }
    }
    if (!found) {
        potentialTargets.emplace_back(nodesToClusters[i]);
        sampleNodeOfPotentialTarget.emplace_back(i);
        nrNodesToBeCollocatedInCluster.emplace_back(uf.getSize(i));
        reservationsThatCanBeUsedPerCluster.emplace_back(reservations[uf.find(i)]);
    }
}
if (!potentialTargets.empty()) {
    //find best target from the vector
    unsigned int bestTarget = 0;
    bool valid = clusterCapacities[potentialTargets[0]] + nrNodesToBeCollocatedInCluster[0] +
                 reservationsThatCanBeUsedPerCluster[0] >= totalSpaceNeeded;
    for (unsigned int i = 1; i < potentialTargets.size(); i++) {
        if (nrNodesToBeCollocatedInCluster[i] > nrNodesToBeCollocatedInCluster[bestTarget] &&
            (clusterCapacities[potentialTargets[i]] + nrNodesToBeCollocatedInCluster[i] +
             reservationsThatCanBeUsedPerCluster[i]) >=
            totalSpaceNeeded) {
            bestTarget = i;
            valid = true;
        }
    }
    //make sure that resulting target cluster is valid (this might not be the case if it is 0 as that is the default)
    if (valid) {
        auto bestTargetCluster = potentialTargets[bestTarget];

        //move all the components to the cluster bestTargetCluster
        moveSetsToCluster(setIds, bestTargetCluster, sampleNodeOfPotentialTarget[bestTarget]);
        return;
    }
}
//otherwise move all components to a cluster that can contain them all, but does not contain any of them
for (unsigned int i = 0; i < clusterCapacities.size(); i++) {
    if (clusterCapacities[i] >= totalSpaceNeeded) {
        //move all components to cluster i and merge them
        moveSetsToCluster(setIds, i, setIds[0]);
        return;
    }
}*/
//throw std::logic_error("Invalid point in ClusterMapping reached: No migration destination was found!");
}


void ClusterMapping::moveSetsToCluster(const std::vector<unsigned int> &setIds, const unsigned int targetClusterId,
                                       const unsigned int targetSet, bool useReservationOnly) {
    auto oldTargetSetSize = uf.getSize(targetSet);
//free all reservations
    for (auto tmpSet:setIds) {
        if (useReservationOnly && tmpSet == targetSet) {
            continue;
        }
        clusterCapacities[nodesToClusters[uf.find(tmpSet)]] += reservations[uf.find(tmpSet)];
        reservations[uf.find(tmpSet)] = 0;
    }
    for (auto tmpSet:setIds) {
        moveToCluster(uf.find(tmpSet), targetClusterId, useReservationOnly);
    }
    for (auto tmpSet: setIds) {
        uf.unionSet(tmpSet, targetSet);
    }
    if (useReservationOnly) {
        reservations[uf.find(targetSet)] -= (uf.getSize(targetSet) - oldTargetSetSize);
    } else {
        reservations[uf.find(targetSet)] = computeReservation(uf.getSize(targetSet));
    }
#ifdef USE_DEBUG
    if (clusterCapacities[targetClusterId] < reservations[uf.find(targetSet)]) {
        throw std::logic_error("Invalid target cluster: capacity is not enough for reservations.");
    }
#endif
    if (!useReservationOnly) {
        clusterCapacities[targetClusterId] -= reservations[uf.find(targetSet)];
    }
}


void ClusterMapping::moveToCluster(const unsigned int x, const unsigned int clusterId, bool resUsed) {
    if (nodesToClusters[x] == clusterId) {
        return;
    }

    auto compOfX = uf.getSetOf(x);
    clusterCapacities[nodesToClusters[x]] += (unsigned int) compOfX.size();
#ifdef USE_DEBUG
    auto oldCapacity = clusterCapacities[clusterId];
#endif
    if (!resUsed) {
        clusterCapacities[clusterId] -= (unsigned int) compOfX.size();
    }
#ifdef USE_DEBUG
    if (clusterCapacities[clusterId] > oldCapacity) {
        std::cout << "Capacity grew as nodes were migrated to a cluster!" << std::endl;
    }
#endif
    nrMoves += (unsigned int) compOfX.size();
    for (auto node: compOfX) {
        nodesToClusters[node] = clusterId;
    }
}


void ClusterMapping::check_server_capacities() {
    auto space_used_per_server = std::vector<unsigned int>(clusterNr, 0);
    for (auto cluster:nodesToClusters) {
        space_used_per_server[cluster]++;
    }
    for (unsigned int node = 0; node < nodesToClusters.size(); node++) {
        space_used_per_server[nodesToClusters[node]] += reservations[node];
    }

//check that no server is over capacity
    unsigned int currServer = 0;
    for (auto i: space_used_per_server) {
        if (i > augmentation * clusterSize) {
            throw std::runtime_error("Server " + std::to_string(currServer) + " is over capacity!");
        }
        if (augmentation * clusterSize - i != clusterCapacities[currServer]) {
            throw std::runtime_error("Server " + std::to_string(currServer) + " has wrong capacity!");
        }
        currServer++;
    }
}