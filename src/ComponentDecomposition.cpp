//
// Created by Tobias on 22/03/2020.
//

#include "ComponentDecomposition.h"

namespace decomposition_tree {

    ComponentDecomposition::ComponentDecomposition(unsigned int n, unsigned int alpha, double augmentation,
                                                   unsigned int clusterSize, unsigned int clusterNr,
                                                   bool initRandomly) : alpha(alpha), g(Graph(n)), clusterMapping(
            ClusterMapping(n, clusterSize, clusterNr, augmentation, augmentation - 2, initRandomly)) {
        components = std::vector<std::vector<node_>>(n, std::vector<node_>());
        for (node_ i = 0; i < n; i++) {
            components[i].emplace_back(i);
        }
        this->totalTimePassed = std::chrono::microseconds(0);
    }

    ComponentDecomposition::~ComponentDecomposition() = default;

    void ComponentDecomposition::addEdge(const decomposition_tree::node_ a, const decomposition_tree::node_ b) {
        g.addEdge(Edge(a, b, 1));
    }


    void
    ComponentDecomposition::updateDecomposition(const decomposition_tree::node_ a, const decomposition_tree::node_ b) {
        std::vector<MergeComponent *> currComponents = std::vector<MergeComponent *>();
        auto nodesToMergeComp = std::vector<unsigned int>(g.getNodesNr(), 0);
        constructComponentGraph(currComponents, nodesToMergeComp);
        this->components = std::vector<std::vector<node_ >>();
        //saves the location of each component w.r.t. the heap
        //std::vector<COMPONENT_STATE> compLocation = std::vector<COMPONENT_STATE>(currComponents.size(), OUTSIDE);
        unsigned int nrResult = 0;
        //saves whether components should be considered in the next iteration
        auto validForNextIt = std::vector<bool>(currComponents.size(), true);
#ifdef USE_DEBUG
        auto debugComponents = std::vector<std::vector<node_>>();
        for (auto comp:currComponents) {

            if (validForNextIt[comp->getNumber()]) {
                debugComponents.emplace_back(comp->getNodes());
            }
        }
        for (auto &comp:components) {
            debugComponents.emplace_back(comp);
        }
        checkComponents(debugComponents);
#endif
        decomposition(validForNextIt, currComponents, nrResult);

#ifdef USE_DEBUG
        //std::cout << "second check in updateDecomposition\n";
        debugComponents = std::vector<std::vector<node_>>();
        for (auto comp:currComponents) {
            if (validForNextIt[comp->getNumber()]) {
                debugComponents.emplace_back(comp->getNodes());
            }
        }
        for (auto &comp:components) {
            debugComponents.emplace_back(comp);
        }
        checkComponents(debugComponents);
#endif

        //check connectivities of all the components as they might have been merged prematurely
        auto check_queue = std::queue<std::vector<unsigned int>>();
        for (auto comp: currComponents) {
            if (validForNextIt[comp->getNumber()]) {
                if (comp->getNodes().size() == 1) {
                    this->components.emplace_back(comp->getNodes());
                } else {
                    check_queue.push(comp->getNodes());
                }
                validForNextIt[comp->getNumber()] = false;
            }
        }
        while (!check_queue.empty()) {
            auto comp = check_queue.front();
            check_queue.pop();
            auto singleComponents = std::vector<MergeComponent *>(comp.size(), nullptr);
            unsigned int pos = 0;
            //turn comp into singleton components
            for (auto i: comp) {
                singleComponents[pos] = new MergeComponent(pos, i);
                pos++;
            }
            //add edges
            for (unsigned int i = 0; i < singleComponents.size(); i++) {
                for (auto e: g.getEdges(singleComponents[i]->getNodes()[0])) {
                    for (unsigned int endIndex = i + 1; endIndex < singleComponents.size(); endIndex++) {
                        if (singleComponents[endIndex]->getNodes()[0] == e->getEnd()) {
                            Edge *e1 = new Edge(i, endIndex, e->getWeight());
                            Edge *e2 = new Edge(endIndex, i, e->getWeight());
                            e1->setReverse(e2);
                            e2->setReverse(e1);
                            singleComponents[i]->addEdgeNoChecks(e1);
                            singleComponents[endIndex]->addEdgeNoChecks(e2);

                        }
                    }
                }
            }
            auto tmpValidForNextIt = std::vector<bool>(singleComponents.size(), true);
            decomposition(tmpValidForNextIt, singleComponents, nrResult);
            if (nrResult == 1 && tmpValidForNextIt[singleComponents[0]->getNumber()]) {
                this->components.emplace_back(comp);
            } else if (nrResult > 1) {
                for (auto c: singleComponents) {
                    if (tmpValidForNextIt[c->getNumber()]) {
                        check_queue.push(c->getNodes());
                    }
                }
            }
            for (auto c: singleComponents) {
                delete c;
            }
        }
#ifdef USE_DEBUG
        //std::cout << "third check in updateDecomposition\n";
        debugComponents = std::vector<std::vector<node_>>();
        for (auto comp:currComponents) {
            if (validForNextIt[comp->getNumber()]) {
                debugComponents.emplace_back(comp->getNodes());
            }
        }
        for (auto &comp:components) {
            debugComponents.emplace_back(comp);
        }
        checkComponents(debugComponents);
#endif
        for (auto c: currComponents) {
            delete c;
        }
    }


    void ComponentDecomposition::insertAndUpdate(const decomposition_tree::node_ a, const decomposition_tree::node_ b,
                                                 STEP_ACTION &stepAction, long &ms) {
        if (clusterMapping.areInSameComponent(a, b)) {
            stepAction = SKIPPED;
            ms = 0;
            return;
        }
#ifdef USE_DEBUG
        checkComponents(this->components);
        if (!clusterMapping.internalStructuresSameAsComps(this->components)) {
            throw std::logic_error("initial components differ from cluster mapping");
        }
        auto compNrBefore = this->components.size();
#endif
        addEdge(a, b);
        auto start = std::chrono::high_resolution_clock::now();
        updateDecomposition(a, b);
#ifdef USE_DEBUG
        if (compNrBefore < this->components.size()) {
            throw std::logic_error("Number of components increased upon edge insertion");
        }
#endif
        stepAction = REQUEST;
        auto toDel = std::vector<std::vector<node_>>();
        bool migration = false;
        clusterMapping.updateMapping(components, toDel, migration);
        if (migration) {
            stepAction = MIGRATION;
        }

        if (!toDel.empty()) {
            stepAction = DELETION;
            for (auto comp: toDel) {
                deleteComponent(comp);
            }
            updateDecomposition(a, b);
            toDel = std::vector<std::vector<node_>>();
            clusterMapping.updateMapping(components, toDel, migration);
            if (!toDel.empty()) {
                throw std::runtime_error("Component too large after deletion!");
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        totalTimePassed += std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
#ifdef USE_DEBUG
        checkComponents(this->components);
        if (!clusterMapping.internalStructuresSameAsComps(this->components)) {
            throw std::logic_error("components differ from cluster mapping after update");
        }
#endif
    }


    std::vector<unsigned int> ComponentDecomposition::getMapping() const {
        return clusterMapping.getMapping();
    }


    void ComponentDecomposition::printInfo() const {}


    long ComponentDecomposition::getMilliseconds() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(totalTimePassed).count();
    }

    void ComponentDecomposition::deleteComponent(std::vector<node_> &comp) {
        for (unsigned int i = 1; i < comp.size(); i++) {
            this->components.emplace_back(std::vector<node_>(1, comp[i]));
        }
        for (unsigned int i = 0; i < components.size(); i++) {
            if (components[i][0] == comp[0]) {
                components[i] = std::vector<node_>(1, comp[0]);
                break;
            }
        }
        for (auto node: comp) {
            g.removeAllEdgesOfNode(node);
        }
    }


    void ComponentDecomposition::decomposition(std::vector<bool> &validForNextIt,
                                               std::vector<MergeComponent *> &currComponents, unsigned int &nrResult) {
        //saves the location of each component w.r.t. the heap
        std::vector<COMPONENT_STATE> compLocation = std::vector<COMPONENT_STATE>(currComponents.size(), OUTSIDE);
        nrResult = currComponents.size();

        //k core based optimization
        bool change = true;
        while (change) {
            change = false;
            for (auto c:currComponents) {
                if (c->getDegree() < alpha && validForNextIt[c->getNumber()]) {
                    change = true;
                    cutComponent(c, currComponents);
                    validForNextIt[c->getNumber()] = false;
                    this->components.emplace_back(c->getNodes());
                }
            }
        }

        // start with one component, build a linear heap with all nodes connected to it and either merge or split
        unsigned int smallestCutValue = alpha;
        for (node_ cN = 0; cN < currComponents.size(); cN++) {
            if (compLocation[cN] == INSERTED || !validForNextIt[cN]) {
                continue;
            }
            //continue reducing the degree of the current component until it is zero
            while (currComponents[cN]->getDegree() > 0) {
                //build LinearHeap
                ListLinearHeap heap = ListLinearHeap(g.getNodesNr(), alpha);
                for (unsigned int cNumber = 0; cNumber < currComponents.size(); cNumber++) {
                    compLocation[cNumber] = OUTSIDE;
                }
                //insert current component into the heap
                heap.insert(currComponents[cN]->getNumber(), 0);
                compLocation[currComponents[cN]->getNumber()] = INSERTED;
                std::vector<unsigned int> Q = std::vector<unsigned int>();
                std::vector<unsigned int> cutValues = std::vector<unsigned int>();
                node_ max_id = 0;
                node_ max_key = 0;
                while (!heap.isEmpty()) {
                    heap.extract_max(max_id, max_key);
                    compLocation[max_id] = REMOVED;
                    //insert all neighbors of max_id into the heap
                    for (Edge *e: currComponents[max_id]->getEdges()) {
                        if (compLocation[e->getEnd()] == INSERTED) {
                            unsigned int oldWeight = heap.remove(e->getEnd());
                            unsigned int weight = e->getWeight() + oldWeight < alpha ? e->getWeight() + oldWeight
                                                                                     : alpha;
                            heap.insert(e->getEnd(), weight);
                        } else if (compLocation[e->getEnd()] == OUTSIDE) {
                            compLocation[e->getEnd()] = INSERTED;
                            unsigned int weight = e->getWeight() < alpha ? e->getWeight() : alpha;
                            heap.insert(e->getEnd(), weight);
                        }
                        //do nothing if endpoint has been removed already
                    }
                    cutValues.emplace_back(max_key < alpha ? max_key : alpha);
                    Q.emplace_back(max_id);
                }

                //merge or cut the last two elements
                MergeComponent *secondToLast = currComponents[Q[Q.size() - 2]];
                MergeComponent *last = currComponents[Q[Q.size() - 1]];
                if (max_key < smallestCutValue) {
                    smallestCutValue = max_key;
                }
                if (max_key < alpha) {
                    //keep cutting starting from the end of Q
                    unsigned long posInQ = Q.size() - 1;
                    while (cutValues[posInQ] < alpha && posInQ > 0) {
                        last = currComponents[Q[posInQ--]];
                        //nrResult -= 1;
                        cutComponent(last, currComponents);
                    }
                } else {
                    nrResult -= 1;
                    mergeComponents(secondToLast, last, validForNextIt, currComponents);
                }
            }//end while
        }
    }


    void ComponentDecomposition::constructComponentGraph(std::vector<MergeComponent *> &currComponents,
                                                         std::vector<unsigned int> &nodesToMergeComp) const {
        unsigned int compNr = 0;
        for (auto comp: this->components) {

            auto curr = new MergeComponent(compNr, comp[0]);
            nodesToMergeComp[comp[0]] = compNr;
            for (unsigned int i = 1; i < comp.size(); i++) {
                curr->addNodeNoChecks(comp[i]);
                nodesToMergeComp[comp[i]] = compNr;
            }
            currComponents.emplace_back(curr);
            compNr++;
        }

        //add edges
        for (auto c:currComponents) {
            for (auto i: c->getNodes()) {
                for (auto e: g.getEdges(i)) {
                    //check if both end points are inside the current sub-graph
                    unsigned int startInComp = nodesToMergeComp[e->getStart()];
                    unsigned int endInComp = nodesToMergeComp[e->getEnd()];
                    //only add edge from component with lower index to avoid unnecessary checks
                    if (startInComp < endInComp) {
                        MergeComponent *startComp = currComponents[startInComp];
                        MergeComponent *endComp = currComponents[endInComp];
                        Edge *e1 = new Edge(startInComp, endInComp, e->getWeight());
                        Edge *e2 = new Edge(endInComp, startInComp, e->getWeight());
                        e1->setReverse(e2);
                        e2->setReverse(e1);
                        startComp->addEdgeNoChecks(e1);
                        endComp->addEdgeNoChecks(e2);
                    }
                }
            }
        }
    }


    void ComponentDecomposition::cutComponent(decomposition_tree::MergeComponent *m,
                                              std::vector<MergeComponent *> &currComponents) const {
        for (Edge *e:m->getEdges()) {
            currComponents[e->getEnd()]->delEdge(m->getNumber());
        }
        m->delAllEdges();
    }


    void ComponentDecomposition::mergeComponents(decomposition_tree::MergeComponent *m1,
                                                 decomposition_tree::MergeComponent *m2,
                                                 std::vector<bool> &validForNextIt,
                                                 std::vector<MergeComponent *> &currComponents) const {
        validForNextIt[m2->getNumber()] = false;
        for (unsigned int i:m2->getNodes()) {
            m1->addNodeNoChecks(i);
        }
        //adjust edges: redirect edges of m2 to m1, delete edge between m1 and m2 and delete all edges of m2
        for (Edge *e:m2->getEdges()) {
            Edge *eSecondToLast = new Edge(m1->getNumber(), e->getEnd(), e->getWeight());
            m1->mergeEdge(eSecondToLast);
            MergeComponent *targetComponent = currComponents[e->getEnd()];
            node_ weight = targetComponent->delEdge(m2->getNumber());
            Edge *eTargetComponent = new Edge(targetComponent->getNumber(), m1->getNumber(),
                                              weight);
            targetComponent->mergeEdge(eTargetComponent);
        }
        m1->delEdge(m2->getNumber());
        m2->delAllEdges();
    }


    void ComponentDecomposition::checkComponents(std::vector<std::vector<node_>> comps) {
        //std::cout << "checking components\n";
        auto nodeFound = std::vector<bool>(g.getNodesNr(), false);
        for (auto &comp: comps) {
            if (comp.empty()) {
                throw std::runtime_error("Empty component!");
            }
            for (auto node:comp) {
                if (nodeFound[node]) {
                    throw std::runtime_error("Node twice in components");
                } else {
                    nodeFound[node] = true;
                }
            }
        }
        for (unsigned int i = 0; i < nodeFound.size(); i++) {
            if (!nodeFound[i]) {
                throw std::runtime_error("node " + std::to_string(i) + " is in no component.");
            }
        }
    }
}