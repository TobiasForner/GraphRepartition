//
// Created by tobias on 21.10.19.
//

#include <fstream>
#include "DecompositionTree.h"

namespace decomposition_tree {

    DecompositionTree::DecompositionTree(unsigned int n, unsigned const int alpha, double augmentation,
                                         unsigned int clusterSize, unsigned int clusterNr, bool initRandomly,
                                         bool tryOpt, DELETION_METHOD del_method, COMP_GRAPH_CONSTRUCTION comp_const,
                                         bool insertBetweenServers) :
            insertBetweenServers(insertBetweenServers), deletionMethod(del_method),
            compGraphConstruction(comp_const), root(nullptr), leaves(std::vector<TreeLeaf>()),
            nodesToLeaves(std::vector<node_>()), edges(std::vector<std::shared_ptr<DoublyLinkedList<Edge *>>>()),
            clusterMapping(ClusterMapping(n, clusterSize, clusterNr, augmentation, augmentation - 2.0, initRandomly)) {
        root = new Tree<TreeNode *>(std::vector<Tree<TreeNode *> *>(), nullptr);
        alphaConnectedComponents = std::vector<std::vector<node_>>();

        this->edgeCountFile = "edge_count_file.csv";
        auto file = std::ofstream(edgeCountFile);
        file << "number of edges" << std::endl;
        file.close();
        this->alpha = alpha;
        this->tryOpt = tryOpt;
#ifdef COLLECT_STATS
        accessLevelCount = std::vector<unsigned int>(alpha + 1, 0);
        nrSkipped = 0;
        nrDeleted = 0;
#endif
        auto rootData = new TreeNode(0, n - 1, 0);
        root->setData(rootData);
        totalTimePassed = std::chrono::microseconds(0);

        for (node_ i = 0; i < n; i++) {
            edges.emplace_back(std::make_shared<DoublyLinkedList<Edge *>>());
            leaves.emplace_back(TreeLeaf(i));
            nodesToLeaves.emplace_back(i);
            alphaConnectedComponents.emplace_back(std::vector<node_>(1, i));
        }
        if (initRandomly) {
            std::random_device rng;
            std::shuffle(leaves.begin(), leaves.end(), std::default_random_engine(rng()));
            for (unsigned int i = 0; i < leaves.size(); i++) {
                nodesToLeaves[leaves[i].getNumber()] = i;
            }
        }
    }

    DecompositionTree::DecompositionTree(unsigned int n, unsigned const int alpha,
                                         std::vector<DoublyLinkedList<Edge *>> &edges, double augmentation,
                                         unsigned int clusterSize, unsigned int clusterNr, bool initRandomly,
                                         bool tryOpt, DELETION_METHOD del_method, COMP_GRAPH_CONSTRUCTION comp_const,
                                         bool insertBetweenServers)
            : insertBetweenServers(insertBetweenServers), deletionMethod(del_method),
              compGraphConstruction(comp_const),
              root(nullptr),
              leaves(std::vector<TreeLeaf>()),
              nodesToLeaves(std::vector<node_>()),
              clusterMapping(
                      ClusterMapping(n, clusterSize, clusterNr, augmentation, augmentation - 2.0, initRandomly)) {
        root = new Tree<TreeNode *>(std::vector<Tree<TreeNode *> *>(), nullptr);
        auto rootData = new TreeNode(0, n - 1, 0);
        root->setData(rootData);
        this->edgeCountFile = "edge_count_file.csv";
        auto file = std::ofstream(edgeCountFile);
        file << "number of edges" << std::endl;
        file.close();
        this->alpha = alpha;
        this->tryOpt = tryOpt;
#ifdef COLLECT_STATS
        accessLevelCount = std::vector<unsigned int>(alpha + 1, 0);
        nrSkipped = 0;
        nrDeleted = 0;
#endif
        totalTimePassed = std::chrono::microseconds(0);
        assert(edges.size() == n);
        this->edges = std::vector<std::shared_ptr<DoublyLinkedList<Edge *>>>();
        for (auto &edgeList:edges) {
            this->edges.emplace_back(std::make_shared<DoublyLinkedList<Edge *>>());
            for (auto &node: edgeList) {
                this->edges[this->edges.size() - 1]->add(node.getData());
            }
        }
        leaves = std::vector<TreeLeaf>();
        nodesToLeaves = std::vector<node_>();
        alphaConnectedComponents = std::vector<std::vector<node_>>();

        for (node_ i = 0; i < n; i++) {
            leaves.emplace_back(TreeLeaf(i));
            nodesToLeaves.emplace_back(i);
            alphaConnectedComponents.emplace_back(std::vector<node_>(1, i));
        }
        if (initRandomly) {
            std::random_device rng;
            std::shuffle(leaves.begin(), leaves.end(), std::default_random_engine(rng()));
            for (unsigned int i = 0; i < leaves.size(); i++) {
                nodesToLeaves[leaves[i].getNumber()] = i;
            }
        }
    }

    DecompositionTree::~DecompositionTree() {
        //delete all data points
        deleteTreeData(root);
        for (auto &edgeList:edges) {
            for (auto &listNode:*edgeList) {
                listNode.getData()->setReverse(nullptr);
                delete (listNode.getData());
            }
        }
        delete (root);
    }


    void DecompositionTree::addEdge(node_ a, node_ b) {
        while (a >= leaves.size() || b >= leaves.size()) {
            TreeLeaf tmp = TreeLeaf((unsigned int) leaves.size());
            leaves.emplace_back(tmp);
            resetRoot();
            edges.emplace_back(std::make_shared<DoublyLinkedList<Edge *>>());
            nodesToLeaves.emplace_back(leaves.size() - 1);
        }
        //search shorter edge list
        auto listToSearch = edges[b];
        node_ target = a;
        if (edges[a]->getSize() < listToSearch->getSize()) {
            listToSearch = edges[a];
            target = b;
        }
        for (auto e: *listToSearch) {
            Edge *data = e.getData();
            if (data->getEnd() == target) {
                //increase edge and reverse edge weight by one
                data->increase_weight(1);
                data->getReverse()->increase_weight(1);
                return;
            }
        }
        Edge *e1 = new Edge(a, b, 1);
        Edge *e2 = new Edge(b, a, 1);
        e1->setReverse(e2);
        e2->setReverse(e1);
        edges[a]->add(e1);
        edges[b]->add(e2);
    }

    Tree<TreeNode *> *DecompositionTree::findSmallestSubgraph(decomposition_tree::node_ a,
                                                              decomposition_tree::node_ b) {
        auto currentRoot = root;
        bool update = true;
#ifdef COLLECT_STATS
        unsigned int level = 0;
#endif
        while (update) {
            update = false;
            for (auto tree:currentRoot->getChildren()) {
                auto data = tree->getData();
                if (containsNode(data, a) && containsNode(data, b)) {
                    currentRoot = tree;
#ifdef COLLECT_STATS
                    level++;
#endif
                    update = true;
                    break;
                }
            }
        }
#ifdef COLLECT_STATS
        accessLevelCount[level]++;
#endif
        return currentRoot;
    }

    void DecompositionTree::updateDecomposition(node_ a, node_ b) {
        auto currentRoot = findSmallestSubgraph(a, b);

        std::queue<Tree<TreeNode *> *> Q = std::queue<Tree<TreeNode *> *>();
        Q.push(currentRoot);
        //iteratively update connectivities and decompose into sub-graphs
        while (!Q.empty()) {
            auto currentTree = Q.front();
            Q.pop();
            //if tree has connectivity alpha delete children and continue
            if (currentTree->getData()->getConnectivity() >= alpha) {
                for (auto t: currentTree->getChildren()) {
                    deleteTreeData(t);
                    delete (t);
                }
                currentTree->setChildren(std::vector<Tree<TreeNode *> *>());
                continue;
            }
            TreeNode *current = currentTree->getData();
            unsigned int smallestCut;
            unsigned int newConnectivity = current->getConnectivity() + 1;
            std::vector<TreeNode *> result = decomposition(currentTree, current->getConnectivity() + 1, smallestCut);

            //update the connectivity of current
            if (current->getConnectivity() < smallestCut) {
                current->setConnectivity(smallestCut);
            }
            //delete children of currentTree
            auto childrenVec = currentTree->getChildren();
            currentTree->setChildren(std::vector<Tree<TreeNode *> *>());
            for (auto t:childrenVec) {
                deleteTreeData(t);
                delete (t);
            }
            if (result.size() == 1) {
                newConnectivity = smallestCut + 1;
                if (newConnectivity > alpha) {
                    for (TreeNode *t: result) {
                        delete (t);
                    }
                    continue;
                }
            }

            //new children of current
            auto newChildren = std::vector<Tree<TreeNode *> *>();
            //queue used to compute the new children of current
            auto children_queue = std::queue<Tree<TreeNode *> *>();
            for (TreeNode *t: result) {
                children_queue.push(new Tree<TreeNode *>(t));
            }

            //decompose children w.r.t. the new connectivity to make sure that theirs is greater than that of the parent
            while (!children_queue.empty()) {
                auto tmp = children_queue.front();
                children_queue.pop();
                std::vector<TreeNode *> tmpDecomposition = decomposition(tmp, newConnectivity, smallestCut);
                if (tmp->getData()->getConnectivity() < smallestCut) {
                    tmp->getData()->setConnectivity(smallestCut);
                }
                if (tmpDecomposition.size() == 1) {
                    //found sub-component of higher connectivity than parent, hence it is a valid child
                    auto tmpTree = new Tree<TreeNode *>(tmpDecomposition[0]);
                    delete (tmp->getData());
                    delete (tmp);
                    newChildren.emplace_back(tmpTree);
                    if (smallestCut < alpha) {
                        Q.push(tmpTree);
                    }
                } else {
                    //components from tmpDecomposition still need to be re-checked to be sure that their connectivity is greater than that of the parent
                    delete (tmp->getData());
                    delete (tmp);
                    for (auto tmpC:tmpDecomposition) {
                        children_queue.push(new Tree<TreeNode *>(tmpC));
                    }
                }
            }
            currentTree->setChildren(newChildren);
        }

        alphaConnectedComponents = std::vector<std::vector<node_>>();
        recomputeAlphaConnectedComponents(alphaConnectedComponents);
    }

    void DecompositionTree::insertAndUpdate(node_ a, node_ b, STEP_ACTION &stepAction, long &ms) {
        if ((insertBetweenServers && clusterMapping.areMappedToSameCluster(a, b)) ||
            (!insertBetweenServers && clusterMapping.areInSameComponent(a, b))) {
#ifdef COLLECT_STATS
            nrSkipped++;
#endif
            stepAction = SKIPPED;
            ms = 0;
            //count edges
            /*
            unsigned int edges_count = 0;
            for (auto &e:edges) {
                edges_count += e->getSize();
            }
            std::ofstream file;
            file.open(this->edgeCountFile, std::ios::app);
            file << "\n" + std::to_string(edges_count / 2);
            file.close();*/
            return;
        }
        stepAction = REQUEST;
        addEdge(a, b);
        auto start = std::chrono::high_resolution_clock::now();
        updateDecomposition(a, b);
        auto toDel = std::vector<std::vector<node_ >>();
        bool migration = false;
        clusterMapping.updateMapping(alphaConnectedComponents, toDel, migration);
        if (migration) {
            stepAction = MIGRATION;
        }
        for (const auto &comp:toDel) {
#ifdef COLLECT_STATS
            ++nrDeleted;
#endif
            delAlphaConnectedComponent(comp);
            stepAction = DELETION;
        }
        auto end = std::chrono::high_resolution_clock::now();
        totalTimePassed += std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
#ifdef USE_DEBUG
        bool boo = clusterMapping.internalStructuresSameAsComps(alphaConnectedComponents);
        if (!boo) {
            std::cerr << "ClusterMapping differs from alpha connected components!" << std::endl;
        }
        checkWholeTree();
#endif
        //count edges
        /*
        unsigned int edges_count = 0;
        for (auto &e:edges) {
            edges_count += e->getSize();
        }
        std::ofstream file;
        file.open(this->edgeCountFile, std::ios::app);
        file << "\n" + std::to_string(edges_count / 2);
        file.close();*/
    }

    std::vector<unsigned int> DecompositionTree::getMapping() const {
        return clusterMapping.getMapping();
    }

    void DecompositionTree::printInfo() const {
#ifdef LOGGING_ENABLED
        //nr of tree nodes per connectivity
        std::vector<unsigned int> connectivityCount = std::vector<unsigned int>(alpha + 1, 0);
        auto levelCount = std::vector<unsigned int>(alpha + 1, 0);
        auto degreePerLevel = std::vector<unsigned int>(alpha + 1, 0);

        auto q2 = std::queue<std::pair<Tree<TreeNode *> *, unsigned int>>();
        q2.push(std::pair<Tree<TreeNode *> *, unsigned int>(root, 0));
        while (!q2.empty()) {
            auto e = q2.front();
            q2.pop();
            levelCount[e.second]++;
            degreePerLevel[e.second] += (unsigned int) e.first->getChildren().size();
            connectivityCount[e.first->getData()->getConnectivity()]++;
            for (auto t: e.first->getChildren()) {
                q2.push(std::pair<Tree<TreeNode *> *, unsigned int>(t, e.second + 1));
            }
        }
        //nr of tree nodes and average unweighted degree per level
        spdlog::info("DecompositionTree current structure data:");
        spdlog::info("count per connectivity:");
        for (unsigned i = 0; i < connectivityCount.size(); i++) {
            spdlog::info("connectivity " + std::to_string(i) + ": " + std::to_string(connectivityCount[i]));
        }
        spdlog::info("count per level:");
        for (unsigned int i = 0; i < levelCount.size(); i++) {
            spdlog::info("level " + std::to_string(i) + ": " + std::to_string(levelCount[i]));
        }
        spdlog::info("average unweighted degree per level:");
        for (unsigned int i = 0; i < levelCount.size(); i++) {
            spdlog::info("level " + std::to_string(i) + ": " +
                         std::to_string((1.0 * degreePerLevel[i]) / ((1.0) * levelCount[i])));
        }
#ifdef COLLECT_STATS
        spdlog::info("number of accesses per level:");
        for (unsigned int i = 0; i < levelCount.size(); i++) {
            spdlog::info("level " + std::to_string(i) + ": " + std::to_string(accessLevelCount[i]));
        }

        spdlog::info("Skipped: " + std::to_string(nrSkipped) +
                     " updates because both end points were matched to the same cluster");
        spdlog::info("Deleted a total of " + std::to_string(nrDeleted) + " components.");
#endif
        spdlog::info("DecompositionTree updateDecomposition time: " +
                     std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(totalTimePassed).count()) +
                     " (ms)");
#endif
    }

    void DecompositionTree::recomputeAlphaConnectedComponents(std::vector<std::vector<node_>> &target) {
        computeAlphaConnectedComponents(root, target);
    }

    void DecompositionTree::computeAlphaConnectedComponents(Tree<TreeNode *> *startTree,
                                                            std::vector<std::vector<node_>> &target) const {
        //iterate through children of tN and accumulate the alpha connected components
        assert(target.empty());
        auto queue = std::queue<Tree<TreeNode *> *>();
        queue.push(startTree);
        while (!queue.empty()) {
            auto tmp = queue.front()->getData();
            if (tmp->getConnectivity() >= alpha) {
                target.emplace_back(std::vector<node_>(tmp->getEndIndex() - tmp->getStartIndex() + 1, 0));
                for (auto pos = tmp->getStartIndex(); pos <= tmp->getEndIndex(); pos++) {
                    target[target.size() - 1][pos - tmp->getStartIndex()] = leaves[pos].getNumber();
                }
            } else if (!queue.front()->getChildren().empty()) {
                for (auto child: queue.front()->getChildren()) {
                    queue.push(child);
                }
            } else {
                //turn the single nodes of the tree node into singleton components
                for (auto n = tmp->getStartIndex(); n <= tmp->getEndIndex(); n++) {
                    target.emplace_back(std::vector<node_>(1, leaves[n].getNumber()));
                }
            }
            queue.pop();
        }
    }

    void DecompositionTree::delAlphaConnectedComponent(std::vector<node_> comp) {
        if (comp.size() <= 1) {
            throw std::invalid_argument("Component of size 1 or smaller cant be deleted!");
        }
        const auto tree = findSmallestSubgraph(comp[0], comp[1]);
        if (!tree->getChildren().empty()) {
            throw std::invalid_argument(
                    "No valid component specified for deletion: corresponding TreeNode has children!");
        }
        //construct approximate alpha connected components
        auto tmpAlphaComponents = std::vector<std::vector<node_>>();
        for (auto c: alphaConnectedComponents) {
            if (c[0] == comp[0]) {
                for (node_ n: c) {
                    tmpAlphaComponents.emplace_back(std::vector<node_>(1, n));
                }
            } else {
                tmpAlphaComponents.emplace_back(c);
            }
        }
        alphaConnectedComponents = tmpAlphaComponents;
        //delete edges between nodes of the tree node

        const auto treeData = tree->getData();
        if (deletionMethod == OLD) {
            auto deletionQueue = std::queue<Edge *>();
            for (auto i = tree->getData()->getStartIndex(); i <= tree->getData()->getEndIndex(); i++) {
                for (auto edge: *edges[leaves[i].getNumber()]) {
                    if (containsNode(treeData, edge.getData()->getStart()) &&
                        containsNode(treeData, edge.getData()->getEnd()) &&
                        edge.getData()->getEnd() > edge.getData()->getStart()) {
                        deletionQueue.push(edge.getData());
                    }
                }
            }
            while (!deletionQueue.empty()) {
                auto toDelete = deletionQueue.front();
                auto toDelete2 = toDelete->getReverse();
                deletionQueue.pop();
                edges[toDelete->getStart()]->remove(toDelete);
                edges[toDelete2->getStart()]->remove(toDelete2);
                delete (toDelete);
                delete (toDelete2);
            }
        } else {
            Filter<Edge *> *filter;
            if (deletionMethod == CORE_ONLY_FILTER) {
                filter = new EdgeFilterCore(treeData->getStartIndex(), treeData->getEndIndex(), nodesToLeaves);
            } else if (deletionMethod == CORE_HALO_FILTER) {
                filter = new EdgeFilterCoreHalo(treeData->getStartIndex(), treeData->getEndIndex(), nodesToLeaves);
            } else {
                std::cerr << "Invalid deletion method specified! Aborting." << std::endl;
                exit(1);
            }
            auto delVec = std::vector<Edge *>();
            for (auto i = treeData->getStartIndex(); i <= treeData->getEndIndex(); i++) {
                edges[leaves[i].getNumber()]->remove(*filter, delVec);
            }
            for (auto e: delVec) {
                if (deletionMethod == CORE_HALO_FILTER) {
                    //delete reverse of outgoing edges, others are already in delVec
                    if (!containsNode(treeData, e->getEnd())) {
                        edges[e->getEnd()]->remove(e->getReverse());
                        delete (e->getReverse());
                    }
                }
                delete (e);
            }
            delete (filter);
        }


        //recompute structures
        for (auto child: root->getChildren()) {
            deleteTreeData(child);
            delete (child);
        }
        root->setChildren(std::vector<Tree<TreeNode *> *>());
        resetRoot();

        updateDecomposition(leaves[0].getNumber(), leaves[leaves.size() - 1].getNumber());
    }

    void
    DecompositionTree::constructComponentGraph(const Tree<TreeNode *> *tree,
                                               std::vector<MergeComponent *> &components) const {
        auto tN = tree->getData();
        //start with previous alpha connected components already merged
        auto nodesToMergeComp = std::vector<unsigned int>(
                tN->getEndIndex() - tN->getStartIndex() + 1, 0);
        if (compGraphConstruction == ALPHA_COMP &&
            tN->getEndIndex() - tN->getStartIndex() + 1 >= alphaConnectedComponents.size()) {
            unsigned int compNr = 0;
            for (auto comp: alphaConnectedComponents) {
                if (tN->getStartIndex() <= nodesToLeaves[comp[0]] && tN->getEndIndex() >= nodesToLeaves[comp[0]]) {
                    auto curr = new MergeComponent(compNr, comp[0]);
                    nodesToMergeComp[nodesToLeaves[comp[0]] - tN->getStartIndex()] = compNr;
                    for (unsigned int i = 1; i < comp.size(); i++) {
                        curr->addNodeNoChecks(comp[i]);
                        nodesToMergeComp[nodesToLeaves[comp[i]] - tN->getStartIndex()] = compNr;
                    }
                    components.emplace_back(curr);
                    compNr++;
                }
            }
        } else if (compGraphConstruction == CHILDREN && !tree->getChildren().empty()) {
            //turn each child into one component
            components = std::vector<MergeComponent *>(tree->getChildren().size(), nullptr);
            unsigned int compNr = 0;
            for (auto c: tree->getChildren()) {
                if (c->getData()->getConnectivity() >= tree->getData()->getConnectivity()) {
                    auto comp = new MergeComponent(compNr, leaves[c->getData()->getStartIndex()].getNumber());
                    for (unsigned int pos = c->getData()->getStartIndex(); pos <= c->getData()->getEndIndex(); pos++) {
                        if (pos > c->getData()->getStartIndex()) {
                            comp->addNodeNoChecks(leaves[pos].getNumber());
                        }
                        nodesToMergeComp[nodesToLeaves[leaves[pos].getNumber()] - tN->getStartIndex()] = compNr;
                    }
                    components[compNr] = comp;
                    compNr++;
                } else {
                    auto childAlphaComps = std::vector<std::vector<node_>>();
                    computeAlphaConnectedComponents(c, childAlphaComps);
                    unsigned int nrAdded = 0;
                    for (auto &alphaComp: childAlphaComps) {
                        auto comp = new MergeComponent(compNr, alphaComp[0]);
                        nodesToMergeComp[alphaComp[0]] = compNr;
                        for (unsigned int i = 1; i < alphaComp.size(); i++) {
                            comp->addNode(alphaComp[i]);
                            nodesToMergeComp[i] = compNr;
                        }
                        components[compNr] = comp;
                        compNr++;
                        ++nrAdded;
                        if (nrAdded > 1) {
                            components.emplace_back(nullptr);
                        }
                    }
                }
            }
        } else {
            //construct initial components, each representing one node of tN
            for (unsigned int i = tN->getStartIndex(); i <= tN->getEndIndex(); i++) {
                unsigned int compNr = i - tN->getStartIndex();
                unsigned int nodeNr = leaves[i].getNumber();
                auto newComponent = new MergeComponent(compNr, nodeNr);
                components.emplace_back(newComponent);
                nodesToMergeComp[i - tN->getStartIndex()] = i - tN->getStartIndex();
            }
        }

        //add edges
        for (auto c:components) {
            for (auto i: c->getNodes()) {
                for (auto listNode: *edges[i]) {
                    Edge *e = listNode.getData();
                    //check if both end points are inside the current sub-graph
                    if (containsNode(tN, e->getStart()) && containsNode(tN, e->getEnd())) {
                        unsigned int startInComp = nodesToMergeComp[nodesToLeaves[e->getStart()] - tN->getStartIndex()];
                        unsigned int endInComp = nodesToMergeComp[nodesToLeaves[e->getEnd()] - tN->getStartIndex()];
                        //only add edge from component with lower index to avoid unnecessary checks
                        if (startInComp < endInComp) {
                            MergeComponent *startComp = components[startInComp];
                            MergeComponent *endComp = components[endInComp];
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
    }

    void DecompositionTree::swap(const node_ a, const node_ b) {
        node_ aLeaf = nodesToLeaves[a];
        node_ bLeaf = nodesToLeaves[b];
        node_ aNr = leaves[aLeaf].getNumber();
        leaves[aLeaf].setNumber(leaves[bLeaf].getNumber());
        leaves[bLeaf].setNumber(aNr);
        nodesToLeaves[b] = aLeaf;
        nodesToLeaves[a] = bLeaf;
    }

    void DecompositionTree::cutComponent(decomposition_tree::MergeComponent *m,
                                         std::vector<MergeComponent *> &components) const {
        for (Edge *e:m->getEdges()) {
            components[e->getEnd()]->delEdge(m->getNumber());
        }
        m->delAllEdges();
    }

    void DecompositionTree::mergeComponents(decomposition_tree::MergeComponent *m1,
                                            decomposition_tree::MergeComponent *m2, std::vector<bool> &validForNextIt,
                                            std::vector<MergeComponent *> &components) const {
        //spdlog::debug("[DecompositionTree] Merging components " + std::to_string(m1->getNumber()) + " and " +
        //              std::to_string(m2->getNumber()));
        validForNextIt[m2->getNumber()] = false;
        for (unsigned int i:m2->getNodes()) {
            m1->addNodeNoChecks(i);
        }
        //adjust edges: redirect edges of m2 to m1, delete edge between m1 and m2 and delete all edges of m2
        for (Edge *e:m2->getEdges()) {
            Edge *eSecondToLast = new Edge(m1->getNumber(), e->getEnd(), e->getWeight());
            m1->mergeEdge(eSecondToLast);
            MergeComponent *targetComponent = components[e->getEnd()];
            node_ weight = targetComponent->delEdge(m2->getNumber());
            Edge *eTargetComponent = new Edge(targetComponent->getNumber(), m1->getNumber(),
                                              weight);
            targetComponent->mergeEdge(eTargetComponent);
        }
        m1->delEdge(m2->getNumber());
        m2->delAllEdges();
    }


    std::vector<TreeNode *>
    DecompositionTree::decomposition(const Tree<TreeNode *> *tree, const unsigned int newConnectivity,
                                     unsigned int &smallestCut) {
        auto tN = tree->getData();
        std::vector<TreeNode *> ret;
        //if node represents only on vertex: return it as a single treeNode with connectivity alpha
        if (tN->getStartIndex() == tN->getEndIndex()) {
            auto tmp = new TreeNode(tN->getStartIndex(), tN->getEndIndex(), alpha);
            ret.emplace_back(tmp);
            smallestCut = alpha;
            return ret;
        }

        //construct partition graph
        std::vector<MergeComponent *> components;
        constructComponentGraph(tree, components);
        //saves the location of each component w.r.t. the heap
        std::vector<COMPONENT_STATE> compLocation = std::vector<COMPONENT_STATE>(components.size(), OUTSIDE);

        //saves whether components should be considered in the next iteration
        std::vector<bool> validForNextIt = std::vector<bool>(components.size(), true);

        unsigned int offset = tN->getStartIndex();
        unsigned int start = tN->getStartIndex();
        //k core based optimization
        bool change = true;
        while (change) {
            change = false;
            for (auto c:components) {
                if (c->getDegree() < newConnectivity && validForNextIt[c->getNumber()]) {
                    change = true;
                    cutComponent(c, components);
                    validForNextIt[c->getNumber()] = false;
                    for (unsigned int i: c->getNodes()) {
                        swap(leaves[nodesToLeaves[i]].getNumber(), leaves[offset].getNumber());
                        offset++;
                    }
                    auto nTreeNode = new TreeNode(start, start + (node_) c->getNodes().size() - 1, smallestCut);
                    ret.emplace_back(nTreeNode);
                    start = nTreeNode->getEndIndex() + 1;
                }
            }
        }

        // start with one component, build a linear heap with all nodes connected to it and either merge or split
        unsigned int smallestCutValue = alpha;
        bool notConnected = true;//will be set to true if there are components that are not connected
        for (node_ cN = 0; cN < components.size(); cN++) {
            if (compLocation[cN] == INSERTED || !validForNextIt[cN]) {
                continue;
            }
            //continue reducing the degree of the current component until it is zero
            while (components[cN]->getDegree() > 0) {
                //build LinearHeap
                ListLinearHeap heap = ListLinearHeap(tN->getEndIndex() + 1 - tN->getStartIndex(), alpha);
                for (unsigned int cNumber = 0; cNumber < components.size(); cNumber++) {
                    compLocation[cNumber] = OUTSIDE;
                }
                //insert current component into the heap
                heap.insert(components[cN]->getNumber(), 0);
                compLocation[components[cN]->getNumber()] = INSERTED;
                std::vector<unsigned int> Q = std::vector<unsigned int>();
                std::vector<unsigned int> cutValues = std::vector<unsigned int>();
                node_ max_id = 0;
                node_ max_key = 0;
                while (!heap.isEmpty()) {
                    heap.extract_max(max_id, max_key);
                    compLocation[max_id] = REMOVED;
                    //insert all neighbors of max_id into the heap
                    for (Edge *e: components[max_id]->getEdges()) {
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
                if (cN == 0 && Q.size() == components.size()) {
                    notConnected = false;
                }

                //merge or cut the last two elements
                MergeComponent *secondToLast = components[Q[Q.size() - 2]];
                MergeComponent *last = components[Q[Q.size() - 1]];
                if (max_key < smallestCutValue) {
                    smallestCutValue = max_key;
                }
                if (max_key < newConnectivity) {
                    //keep cutting starting from the end of Q
                    unsigned long posInQ = Q.size() - 1;
                    while (cutValues[posInQ] < newConnectivity && posInQ > 0) {
                        last = components[Q[posInQ--]];
                        cutComponent(last, components);
                    }
                } else {
                    mergeComponents(secondToLast, last, validForNextIt, components);
                }
            }//end while
        }
        if (notConnected) {
            smallestCutValue = 0;
        }
        smallestCut = smallestCutValue;
        //construct returned vector: reorder leaves and turn merge components into TreeNodes
        for (MergeComponent *c:components) {
            if (validForNextIt[c->getNumber()]) {
                //reorganize leaf list
                for (unsigned int i: c->getNodes()) {
                    swap(leaves[nodesToLeaves[i]].getNumber(), leaves[offset].getNumber());
                    offset++;
                }
                auto nTreeNode = new TreeNode(start, start + (node_) c->getNodes().size() - 1, smallestCut);
                ret.emplace_back(nTreeNode);
                start = nTreeNode->getEndIndex() + 1;
            }
            delete (c);
        }
        return ret;
    }

    Tree<TreeNode *> *DecompositionTree::getRoot() {
        return root;
    }

    std::vector<std::shared_ptr<DoublyLinkedList<Edge *>>> DecompositionTree::getEdges() {
        return edges;
    }

    long DecompositionTree::getMilliseconds() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(totalTimePassed).count();
    }

    void DecompositionTree::deleteTreeData(Tree<TreeNode *> *t) {
        delete (t->getData());
        for (auto tmp:t->getChildren()) {
            deleteTreeData(tmp);
        }
    }

    void DecompositionTree::resetRoot() {
        auto rootData = root->getData();
        rootData->setConnectivity(0);
        rootData->setStartIndex(0);
        rootData->setEndIndex((unsigned int) leaves.size() - 1);
    }

    bool DecompositionTree::containsNode(const TreeNode *tN, const decomposition_tree::node_ n) const {
        return tN->getStartIndex() <= nodesToLeaves[n] && tN->getEndIndex() >= nodesToLeaves[n];
    }
}
