//
// Created by tobias on 03.11.19.
//

#include "catch.h"
#include "../src/DecompositionTree.h"

namespace decomposition_tree {

    TEST_CASE("DecompositionTree ConstructorTest", "[DecompositionTree]") {
        auto t = new DecompositionTree(10, 5, 3, 10, 1, true, false);
        REQUIRE(t->getAlpha() == 5);
        REQUIRE(t->getRoot()->getData()->getConnectivity() == 0);
        REQUIRE(t->getRoot()->getData()->getStartIndex() == 0);
        REQUIRE(t->getRoot()->getData()->getEndIndex() == 9);
        REQUIRE(t->getEdges().size() == 10);

        delete (t);

        auto edges = std::vector<DoublyLinkedList<Edge *>>();

        auto e1 = new Edge(0, 1, 4);

        edges.emplace_back();
        edges[0].add(e1);

        auto e2 = new Edge(1, 0, 4);
        edges.emplace_back();
        edges[1].add(e2);

        auto e3 = new Edge(2, 1, 4);
        edges.emplace_back();
        edges[2].add(e3);

        SECTION("tryOpt=false") {
            auto t2 = new DecompositionTree(3, 6, edges, 3, 3, 1, false, false);

            REQUIRE(t2->getAlpha() == 6);
            REQUIRE(t2->getRoot()->getData()->getConnectivity() == 0);
            REQUIRE(t2->getRoot()->getData()->getStartIndex() == 0);
            REQUIRE(t2->getRoot()->getData()->getEndIndex() == 2);
            REQUIRE(t2->getEdges().size() == 3);
            delete (t2);
        }

        SECTION("tryOpt=true") {
            auto t2 = new DecompositionTree(3, 6, edges, 3, 3, 1, false, true);

            REQUIRE(t2->getAlpha() == 6);
            REQUIRE(t2->getRoot()->getData()->getConnectivity() == 0);
            REQUIRE(t2->getRoot()->getData()->getStartIndex() == 0);
            REQUIRE(t2->getRoot()->getData()->getEndIndex() == 2);
            REQUIRE(t2->getEdges().size() == 3);
            delete (t2);
        }
        SECTION("initRandomly=true") {
            auto t2 = new DecompositionTree(3, 6, edges, 3, 3, 1, true, false);

            REQUIRE(t2->getAlpha() == 6);
            REQUIRE(t2->getRoot()->getData()->getConnectivity() == 0);
            REQUIRE(t2->getRoot()->getData()->getStartIndex() == 0);
            REQUIRE(t2->getRoot()->getData()->getEndIndex() == 2);
            REQUIRE(t2->getEdges().size() == 3);
            delete (t2);
        }
    }

    TEST_CASE("DecompositionTree addEdge", "[DecompositionTree]") {
        DecompositionTree t1 = DecompositionTree(4, 3, 3, 4, 1, false, false);
        t1.addEdge(0, 1);
        REQUIRE(t1.getEdges()[0]->getSize() == 1);
        REQUIRE(t1.getEdges()[1]->getSize() == 1);
        REQUIRE(t1.getEdges()[2]->getSize() == 0);
        REQUIRE(t1.getEdges()[3]->getSize() == 0);
        REQUIRE(t1.getEdges()[0]->getFront()->getData()->getWeight() == 1);
        REQUIRE(t1.getEdges()[1]->getFront()->getData()->getWeight() == 1);
        REQUIRE(t1.getEdges()[0]->getFront()->getData()->getStart() == 0);
        REQUIRE(t1.getEdges()[1]->getFront()->getData()->getStart() == 1);
        REQUIRE(t1.getEdges()[0]->getFront()->getData()->getEnd() == 1);
        REQUIRE(t1.getEdges()[1]->getFront()->getData()->getEnd() == 0);

        t1.addEdge(0, 1);
        REQUIRE(t1.getEdges()[0]->getSize() == 1);
        REQUIRE(t1.getEdges()[1]->getSize() == 1);
        REQUIRE(t1.getEdges()[2]->getSize() == 0);
        REQUIRE(t1.getEdges()[3]->getSize() == 0);
        CHECK(t1.getEdges()[0]->getFront()->getData()->getWeight() == 2);
        CHECK(t1.getEdges()[1]->getFront()->getData()->getWeight() == 2);
        REQUIRE(t1.getEdges()[0]->getFront()->getData()->getStart() == 0);
        REQUIRE(t1.getEdges()[1]->getFront()->getData()->getStart() == 1);
        REQUIRE(t1.getEdges()[0]->getFront()->getData()->getEnd() == 1);
        REQUIRE(t1.getEdges()[1]->getFront()->getData()->getEnd() == 0);

        t1.addEdge(1, 3);
        REQUIRE(t1.getEdges()[0]->getSize() == 1);
        REQUIRE(t1.getEdges()[1]->getSize() == 2);
        REQUIRE(t1.getEdges()[2]->getSize() == 0);
        REQUIRE(t1.getEdges()[3]->getSize() == 1);
        REQUIRE(t1.getEdges()[3]->getFront()->getData()->getWeight() == 1);
        REQUIRE(t1.getEdges()[1]->getFront()->getNext()->getData()->getWeight() == 1);
        REQUIRE(t1.getEdges()[1]->getFront()->getNext()->getData()->getStart() == 1);
        REQUIRE(t1.getEdges()[3]->getFront()->getData()->getStart() == 3);
        REQUIRE(t1.getEdges()[1]->getFront()->getNext()->getData()->getEnd() == 3);
        REQUIRE(t1.getEdges()[3]->getFront()->getData()->getEnd() == 1);
        t1.addEdge(1, 4);
        REQUIRE(t1.getEdges().size() == 5);
        REQUIRE(t1.getEdges()[1]->getSize() == 3);
        REQUIRE(t1.getEdges()[4]->getSize() == 1);
        REQUIRE(t1.getRoot()->getData()->getEndIndex() - t1.getRoot()->getData()->getStartIndex() + 1 == 5);

        //insertions between components
        DecompositionTree t2 = DecompositionTree(4, 2, 3, 2, 2, false, false, DecompositionTree::OLD,
                                                 DecompositionTree::NAIVE, false);
        decomposition_tree::STEP_ACTION stepAction;
        long ms;
        t2.insertAndUpdate(0, 1, stepAction, ms);
        REQUIRE(t2.getMapping()[0] == t2.getMapping()[1]);
        CHECK(t2.getEdges()[0]->getSize() == 1);
        CHECK(t2.getEdges()[1]->getSize() == 1);
    }

    TEST_CASE("DecompositionTree updateDecompositionTest1", "[DecompositionTree]") {
        auto edges = std::vector<DoublyLinkedList<Edge *>>();
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());

        SECTION("default case with core only deletion") {
            auto t = DecompositionTree(3, 3, edges, 4, 3, 1, true, false);
            t.updateDecomposition(0, 2);
            REQUIRE(t.getRoot()->getData()->getConnectivity() == 0);
            REQUIRE(t.getRoot()->getChildren().size() == 3);
            for (auto c:t.getRoot()->getChildren()) {
                REQUIRE(c->getData()->getConnectivity() == 3);
            }
        }

        SECTION("case with core only deletion via filters") {
            auto t = DecompositionTree(3, 3, edges, 4, 3, 1, true, false,
                                       DecompositionTree::CORE_ONLY_FILTER);
            t.updateDecomposition(0, 2);
            REQUIRE(t.getRoot()->getData()->getConnectivity() == 0);
            REQUIRE(t.getRoot()->getChildren().size() == 3);
            for (auto c:t.getRoot()->getChildren()) {
                REQUIRE(c->getData()->getConnectivity() == 3);
            }
        }

        SECTION("case with core and halo deletions") {
            auto t = DecompositionTree(3, 3, edges, 4, 3, 1, true, false,
                                       DecompositionTree::CORE_HALO_FILTER);
            t.updateDecomposition(0, 2);
            REQUIRE(t.getRoot()->getData()->getConnectivity() == 0);
            REQUIRE(t.getRoot()->getChildren().size() == 3);
            for (auto c:t.getRoot()->getChildren()) {
                REQUIRE(c->getData()->getConnectivity() == 3);
            }
        }
    }

    TEST_CASE("DecompositionTree updateDecompositionTest2", "[DecompositionTree]") {
        auto edges = std::vector<DoublyLinkedList<Edge *>>();
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        //edges.emplace_back(DoublyLinkedList<Edge &>());
        auto e1 = new Edge(0, 1, 1);
        auto e2 = new Edge(1, 0, 1);
        e1->setReverse(e2);
        e2->setReverse(e1);
        edges[0].add(e1);
        edges[1].add(e2);


        SECTION("default case with core only deletion") {
            auto t = DecompositionTree(2, 3, edges, 3, 2, 1, false, false, DecompositionTree::OLD);
            t.updateDecomposition(0, 2);
            REQUIRE(t.getRoot()->getData()->getConnectivity() == 1);
            REQUIRE(t.getRoot()->getChildren().size() == 2);
            for (auto c:t.getRoot()->getChildren()) {
                REQUIRE(c->getData()->getConnectivity() == 3);
            }
            REQUIRE(t.findSmallestSubgraph(0, 1)->getData()->getConnectivity() == 1);
            REQUIRE(t.findSmallestSubgraph(0, 0)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(1, 1)->getData()->getConnectivity() == 3);
        }

        SECTION("case with core only deletion via filters") {
            auto t = DecompositionTree(2, 3, edges, 3, 2, 1, false, false, DecompositionTree::CORE_ONLY_FILTER);
            t.updateDecomposition(0, 2);
            REQUIRE(t.getRoot()->getData()->getConnectivity() == 1);
            REQUIRE(t.getRoot()->getChildren().size() == 2);
            for (auto c:t.getRoot()->getChildren()) {
                REQUIRE(c->getData()->getConnectivity() == 3);
            }
            REQUIRE(t.findSmallestSubgraph(0, 1)->getData()->getConnectivity() == 1);
            REQUIRE(t.findSmallestSubgraph(0, 0)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(1, 1)->getData()->getConnectivity() == 3);
        }

        SECTION("case with core and halo deletions") {
            auto t = DecompositionTree(2, 3, edges, 3, 2, 1, false, false, DecompositionTree::CORE_HALO_FILTER);
            t.updateDecomposition(0, 2);
            REQUIRE(t.getRoot()->getData()->getConnectivity() == 1);
            REQUIRE(t.getRoot()->getChildren().size() == 2);
            for (auto c:t.getRoot()->getChildren()) {
                REQUIRE(c->getData()->getConnectivity() == 3);
            }
            REQUIRE(t.findSmallestSubgraph(0, 1)->getData()->getConnectivity() == 1);
            REQUIRE(t.findSmallestSubgraph(0, 0)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(1, 1)->getData()->getConnectivity() == 3);
        }
    }

    TEST_CASE("DecompositionTree updateDecompositionTest3", "[DecompositionTree]") {
        auto edges = std::vector<DoublyLinkedList<Edge *>>();
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        Edge *e1 = new Edge(0, 1, 1);
        Edge *e2 = new Edge(1, 0, 1);
        e1->setReverse(e2);
        e2->setReverse(e1);
        edges[0].add(e1);
        edges[1].add(e2);

        SECTION("default case with core only deletion") {
            auto t = DecompositionTree(3, 4, edges, 3, 3, 1, false, false, DecompositionTree::OLD);
            t.updateDecomposition(0, 2);
            REQUIRE(t.getRoot()->getData()->getConnectivity() == 0);
            REQUIRE(t.getRoot()->getChildren().size() == 2);
            REQUIRE(t.findSmallestSubgraph(0, 0)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(1, 1)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(2, 2)->getData()->getConnectivity() == 4);

            REQUIRE(t.findSmallestSubgraph(0, 1)->getData()->getConnectivity() == 1);

            REQUIRE(t.findSmallestSubgraph(0, 2)->getData()->getConnectivity() == 0);
            REQUIRE(t.findSmallestSubgraph(1, 2)->getData()->getConnectivity() == 0);
        }

        SECTION("case with core only deletion via filters") {
            auto t = DecompositionTree(3, 4, edges, 3, 3, 1, false, false, DecompositionTree::CORE_ONLY_FILTER);
            t.updateDecomposition(0, 2);
            REQUIRE(t.getRoot()->getData()->getConnectivity() == 0);
            REQUIRE(t.getRoot()->getChildren().size() == 2);
            REQUIRE(t.findSmallestSubgraph(0, 0)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(1, 1)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(2, 2)->getData()->getConnectivity() == 4);

            REQUIRE(t.findSmallestSubgraph(0, 1)->getData()->getConnectivity() == 1);

            REQUIRE(t.findSmallestSubgraph(0, 2)->getData()->getConnectivity() == 0);
            REQUIRE(t.findSmallestSubgraph(1, 2)->getData()->getConnectivity() == 0);
        }

        SECTION("case with core and halo deletions") {
            auto t = DecompositionTree(3, 4, edges, 3, 3, 1, false, false, DecompositionTree::CORE_HALO_FILTER);
            t.updateDecomposition(0, 2);
            REQUIRE(t.getRoot()->getData()->getConnectivity() == 0);
            REQUIRE(t.getRoot()->getChildren().size() == 2);
            REQUIRE(t.findSmallestSubgraph(0, 0)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(1, 1)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(2, 2)->getData()->getConnectivity() == 4);

            REQUIRE(t.findSmallestSubgraph(0, 1)->getData()->getConnectivity() == 1);

            REQUIRE(t.findSmallestSubgraph(0, 2)->getData()->getConnectivity() == 0);
            REQUIRE(t.findSmallestSubgraph(1, 2)->getData()->getConnectivity() == 0);
        }
    }

    TEST_CASE("DecompositionTree updateDecompositionTest4", "[DecompositionTree]") {
        std::vector<DoublyLinkedList<Edge *>> edges = std::vector<DoublyLinkedList<Edge *>>();
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        Edge *e1 = new Edge(0, 1, 2);
        Edge *e2 = new Edge(1, 0, 2);
        Edge *e3 = new Edge(1, 2, 3);
        Edge *e4 = new Edge(2, 1, 3);
        Edge *e5 = new Edge(2, 0, 2);
        Edge *e6 = new Edge(0, 2, 2);
        Edge *e7 = new Edge(3, 4, 2);
        Edge *e8 = new Edge(4, 03, 2);
        e1->setReverse(e2);
        e2->setReverse(e1);
        e3->setReverse(e4);
        e4->setReverse(e3);
        e5->setReverse(e6);
        e6->setReverse(e5);
        e7->setReverse(e8);
        e8->setReverse(e7);

        edges[0].add(e1);
        edges[0].add(e6);
        edges[1].add(e2);
        edges[1].add(e3);
        edges[2].add(e4);
        edges[2].add(e5);
        edges[3].add(e7);
        edges[4].add(e8);

        SECTION("default case with core only deletion") {
            auto t = DecompositionTree(5, 3, edges, 3, 5, 1, false, false, DecompositionTree::OLD);
            t.updateDecomposition(0, 4);
            REQUIRE(t.getRoot()->getData()->getConnectivity() == 0);
            REQUIRE(t.getRoot()->getChildren().size() == 2);
            for (unsigned int i = 0; i < 3; i++) {
                REQUIRE(t.findSmallestSubgraph(i, i)->getData()->getConnectivity() == 3);
            }
            REQUIRE(t.findSmallestSubgraph(0, 1)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(0, 2)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(1, 2)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(0, 1)->getData()->getEndIndex() -
                    t.findSmallestSubgraph(0, 1)->getData()->getStartIndex() + 1 == 3);

            REQUIRE(t.findSmallestSubgraph(3, 4)->getData()->getConnectivity() == 2);

            REQUIRE(t.findSmallestSubgraph(3, 0)->getData()->getConnectivity() == 0);
        }

        SECTION("case with core only deletion via filters") {
            auto t = DecompositionTree(5, 3, edges, 3, 5, 1, false, false, DecompositionTree::CORE_ONLY_FILTER);
            t.updateDecomposition(0, 4);
            REQUIRE(t.getRoot()->getData()->getConnectivity() == 0);
            REQUIRE(t.getRoot()->getChildren().size() == 2);
            for (unsigned int i = 0; i < 3; i++) {
                REQUIRE(t.findSmallestSubgraph(i, i)->getData()->getConnectivity() == 3);
            }
            REQUIRE(t.findSmallestSubgraph(0, 1)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(0, 2)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(1, 2)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(0, 1)->getData()->getEndIndex() -
                    t.findSmallestSubgraph(0, 1)->getData()->getStartIndex() + 1 == 3);

            REQUIRE(t.findSmallestSubgraph(3, 4)->getData()->getConnectivity() == 2);

            REQUIRE(t.findSmallestSubgraph(3, 0)->getData()->getConnectivity() == 0);
        }

        SECTION("case with core and halo deletions") {
            auto t = DecompositionTree(5, 3, edges, 3, 5, 1, false, false, DecompositionTree::CORE_HALO_FILTER);
            t.updateDecomposition(0, 4);
            REQUIRE(t.getRoot()->getData()->getConnectivity() == 0);
            REQUIRE(t.getRoot()->getChildren().size() == 2);
            for (unsigned int i = 0; i < 3; i++) {
                REQUIRE(t.findSmallestSubgraph(i, i)->getData()->getConnectivity() == 3);
            }
            REQUIRE(t.findSmallestSubgraph(0, 1)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(0, 2)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(1, 2)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(0, 1)->getData()->getEndIndex() -
                    t.findSmallestSubgraph(0, 1)->getData()->getStartIndex() + 1 == 3);

            REQUIRE(t.findSmallestSubgraph(3, 4)->getData()->getConnectivity() == 2);

            REQUIRE(t.findSmallestSubgraph(3, 0)->getData()->getConnectivity() == 0);
        }
    }

    TEST_CASE("DecompositionTree updateDecompositionTest5", "[DecompositionTree]") {
        std::vector<DoublyLinkedList<Edge *>> edges = std::vector<DoublyLinkedList<Edge *>>();
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        Edge *e1 = new Edge(0, 1, 2);
        Edge *e2 = new Edge(1, 0, 2);
        Edge *e3 = new Edge(1, 2, 2);
        Edge *e4 = new Edge(2, 1, 2);
        Edge *e5 = new Edge(2, 0, 2);
        Edge *e6 = new Edge(0, 2, 2);
        Edge *e7 = new Edge(2, 3, 3);
        Edge *e8 = new Edge(3, 2, 3);
        Edge *e9 = new Edge(3, 4, 3);
        Edge *e10 = new Edge(4, 3, 3);
        Edge *e11 = new Edge(4, 5, 3);
        Edge *e12 = new Edge(5, 4, 3);
        Edge *e13 = new Edge(5, 6, 3);
        Edge *e14 = new Edge(6, 5, 3);
        Edge *e15 = new Edge(6, 7, 3);
        Edge *e16 = new Edge(7, 6, 3);
        Edge *e17 = new Edge(7, 3, 3);
        Edge *e18 = new Edge(3, 7, 3);
        e1->setReverse(e2);
        e2->setReverse(e1);
        e3->setReverse(e4);
        e4->setReverse(e3);
        e5->setReverse(e6);
        e6->setReverse(e5);
        e7->setReverse(e8);
        e8->setReverse(e7);
        e9->setReverse(e10);
        e10->setReverse(e9);
        e11->setReverse(e12);
        e12->setReverse(e11);
        e13->setReverse(e14);
        e14->setReverse(e13);
        e15->setReverse(e16);
        e16->setReverse(e15);
        e17->setReverse(e18);
        e18->setReverse(e17);


        edges[0].add(e1);
        edges[0].add(e6);
        edges[1].add(e2);
        edges[1].add(e3);
        edges[2].add(e4);
        edges[2].add(e5);
        edges[2].add(e7);
        edges[3].add(e8);
        edges[3].add(e9);
        edges[4].add(e10);
        edges[4].add(e11);
        edges[5].add(e12);
        edges[5].add(e13);
        edges[6].add(e14);
        edges[6].add(e15);
        edges[7].add(e16);
        edges[7].add(e17);
        edges[3].add(e18);

        SECTION("default case with core only deletion") {
            auto t = DecompositionTree(8, 7, edges, 3, 8, 1, false, false, DecompositionTree::OLD);
            t.updateDecomposition(0, 7);
            REQUIRE(t.getRoot()->getData()->getConnectivity() == 3);
            REQUIRE(t.getRoot()->getChildren().size() == 2);
            for (unsigned int i = 0; i < 8; i++) {
                REQUIRE(t.findSmallestSubgraph(i, i)->getData()->getConnectivity() == 7);
            }
            REQUIRE(t.findSmallestSubgraph(0, 1)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(0, 2)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(1, 2)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(0, 1)->getData()->getEndIndex() -
                    t.findSmallestSubgraph(0, 1)->getData()->getStartIndex() + 1 == 3);
            REQUIRE(t.findSmallestSubgraph(3, 0)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 1)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 2)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 4)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 5)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 6)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 7)->getData()->getConnectivity() == 6);
        }

        SECTION("case with core only deletion via filters") {
            auto t = DecompositionTree(8, 7, edges, 3, 8, 1, false, false, DecompositionTree::CORE_ONLY_FILTER);
            t.updateDecomposition(0, 7);
            REQUIRE(t.getRoot()->getData()->getConnectivity() == 3);
            REQUIRE(t.getRoot()->getChildren().size() == 2);
            for (unsigned int i = 0; i < 8; i++) {
                REQUIRE(t.findSmallestSubgraph(i, i)->getData()->getConnectivity() == 7);
            }
            REQUIRE(t.findSmallestSubgraph(0, 1)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(0, 2)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(1, 2)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(0, 1)->getData()->getEndIndex() -
                    t.findSmallestSubgraph(0, 1)->getData()->getStartIndex() + 1 == 3);
            REQUIRE(t.findSmallestSubgraph(3, 0)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 1)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 2)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 4)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 5)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 6)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 7)->getData()->getConnectivity() == 6);
        }

        SECTION("case with core and halo deletions") {
            auto t = DecompositionTree(8, 7, edges, 3, 8, 1, false, false, DecompositionTree::CORE_HALO_FILTER);
            t.updateDecomposition(0, 7);
            REQUIRE(t.getRoot()->getData()->getConnectivity() == 3);
            REQUIRE(t.getRoot()->getChildren().size() == 2);
            for (unsigned int i = 0; i < 8; i++) {
                REQUIRE(t.findSmallestSubgraph(i, i)->getData()->getConnectivity() == 7);
            }
            REQUIRE(t.findSmallestSubgraph(0, 1)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(0, 2)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(1, 2)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(0, 1)->getData()->getEndIndex() -
                    t.findSmallestSubgraph(0, 1)->getData()->getStartIndex() + 1 == 3);
            REQUIRE(t.findSmallestSubgraph(3, 0)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 1)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 2)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 4)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 5)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 6)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 7)->getData()->getConnectivity() == 6);
        }
    }

    TEST_CASE("DecompositionTree updateDecompositionTest5Swapped", "[DecompositionTree]") {
        //updateDecompositionTest5 with nodes 0 and 7 swapped
        std::vector<DoublyLinkedList<Edge *>> edges = std::vector<DoublyLinkedList<Edge *>>();
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        Edge *e1 = new Edge(7, 1, 2);
        Edge *e2 = new Edge(1, 7, 2);
        Edge *e3 = new Edge(1, 2, 2);
        Edge *e4 = new Edge(2, 1, 2);
        Edge *e5 = new Edge(2, 7, 2);
        Edge *e6 = new Edge(7, 2, 2);
        Edge *e7 = new Edge(2, 3, 3);
        Edge *e8 = new Edge(3, 2, 3);
        Edge *e9 = new Edge(3, 4, 3);
        Edge *e10 = new Edge(4, 3, 3);
        Edge *e11 = new Edge(4, 5, 3);
        Edge *e12 = new Edge(5, 4, 3);
        Edge *e13 = new Edge(5, 6, 3);
        Edge *e14 = new Edge(6, 5, 3);
        Edge *e15 = new Edge(6, 0, 3);
        Edge *e16 = new Edge(0, 6, 3);
        Edge *e17 = new Edge(0, 3, 3);
        Edge *e18 = new Edge(3, 0, 3);
        e1->setReverse(e2);
        e2->setReverse(e1);
        e3->setReverse(e4);
        e4->setReverse(e3);
        e5->setReverse(e6);
        e6->setReverse(e5);
        e7->setReverse(e8);
        e8->setReverse(e7);
        e9->setReverse(e10);
        e10->setReverse(e9);
        e11->setReverse(e12);
        e12->setReverse(e11);
        e13->setReverse(e14);
        e14->setReverse(e13);
        e15->setReverse(e16);
        e16->setReverse(e15);
        e17->setReverse(e18);
        e18->setReverse(e17);

        edges[7].add(e1);
        edges[7].add(e6);
        edges[1].add(e2);
        edges[1].add(e3);
        edges[2].add(e4);
        edges[2].add(e5);
        edges[2].add(e7);
        edges[3].add(e8);
        edges[3].add(e9);
        edges[4].add(e10);
        edges[4].add(e11);
        edges[5].add(e12);
        edges[5].add(e13);
        edges[6].add(e14);
        edges[6].add(e15);
        edges[0].add(e16);
        edges[0].add(e17);
        edges[3].add(e18);

        SECTION("default case with core only deletion") {
            auto t = DecompositionTree(8, 7, edges, 3, 8, 1, false, false, DecompositionTree::OLD);
            t.updateDecomposition(0, 7);
            REQUIRE(t.getRoot()->getData()->getConnectivity() == 3);
            REQUIRE(t.getRoot()->getChildren().size() == 2);
            for (unsigned int i = 0; i < 8; i++) {
                REQUIRE(t.findSmallestSubgraph(i, i)->getData()->getConnectivity() == 7);
            }
            REQUIRE(t.findSmallestSubgraph(7, 1)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(7, 2)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(1, 2)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(7, 1)->getData()->getEndIndex() -
                    t.findSmallestSubgraph(7, 1)->getData()->getStartIndex() + 1 == 3);
            REQUIRE(t.findSmallestSubgraph(3, 7)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 1)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 2)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 4)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 5)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 6)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 0)->getData()->getConnectivity() == 6);
        }

        SECTION("case with core only deletion via filters") {
            auto t = DecompositionTree(8, 7, edges, 3, 8, 1, false, false, DecompositionTree::CORE_ONLY_FILTER);
            t.updateDecomposition(0, 7);
            REQUIRE(t.getRoot()->getData()->getConnectivity() == 3);
            REQUIRE(t.getRoot()->getChildren().size() == 2);
            for (unsigned int i = 0; i < 8; i++) {
                REQUIRE(t.findSmallestSubgraph(i, i)->getData()->getConnectivity() == 7);
            }
            REQUIRE(t.findSmallestSubgraph(7, 1)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(7, 2)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(1, 2)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(7, 1)->getData()->getEndIndex() -
                    t.findSmallestSubgraph(7, 1)->getData()->getStartIndex() + 1 == 3);
            REQUIRE(t.findSmallestSubgraph(3, 7)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 1)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 2)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 4)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 5)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 6)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 0)->getData()->getConnectivity() == 6);
        }

        SECTION("case with core and halo deletions") {
            auto t = DecompositionTree(8, 7, edges, 3, 8, 1, false, false, DecompositionTree::CORE_HALO_FILTER);
            t.updateDecomposition(0, 7);
            REQUIRE(t.getRoot()->getData()->getConnectivity() == 3);
            REQUIRE(t.getRoot()->getChildren().size() == 2);
            for (unsigned int i = 0; i < 8; i++) {
                REQUIRE(t.findSmallestSubgraph(i, i)->getData()->getConnectivity() == 7);
            }
            REQUIRE(t.findSmallestSubgraph(7, 1)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(7, 2)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(1, 2)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(7, 1)->getData()->getEndIndex() -
                    t.findSmallestSubgraph(7, 1)->getData()->getStartIndex() + 1 == 3);
            REQUIRE(t.findSmallestSubgraph(3, 7)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 1)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 2)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 4)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 5)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 6)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 0)->getData()->getConnectivity() == 6);
        }
    }

    TEST_CASE("DecompositionTree updateDecompositionTest5SwappedTryOpt", "[DecompositionTree]") {
        //updateDecompositionTest5 with nodes 0 and 7 swapped
        std::vector<DoublyLinkedList<Edge *>> edges = std::vector<DoublyLinkedList<Edge *>>();
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        edges.emplace_back(DoublyLinkedList<Edge *>());
        Edge *e1 = new Edge(7, 1, 2);
        Edge *e2 = new Edge(1, 7, 2);
        Edge *e3 = new Edge(1, 2, 2);
        Edge *e4 = new Edge(2, 1, 2);
        Edge *e5 = new Edge(2, 7, 2);
        Edge *e6 = new Edge(7, 2, 2);
        Edge *e7 = new Edge(2, 3, 3);
        Edge *e8 = new Edge(3, 2, 3);
        Edge *e9 = new Edge(3, 4, 3);
        Edge *e10 = new Edge(4, 3, 3);
        Edge *e11 = new Edge(4, 5, 3);
        Edge *e12 = new Edge(5, 4, 3);
        Edge *e13 = new Edge(5, 6, 3);
        Edge *e14 = new Edge(6, 5, 3);
        Edge *e15 = new Edge(6, 0, 3);
        Edge *e16 = new Edge(0, 6, 3);
        Edge *e17 = new Edge(0, 3, 3);
        Edge *e18 = new Edge(3, 0, 3);
        e1->setReverse(e2);
        e2->setReverse(e1);
        e3->setReverse(e4);
        e4->setReverse(e3);
        e5->setReverse(e6);
        e6->setReverse(e5);
        e7->setReverse(e8);
        e8->setReverse(e7);
        e9->setReverse(e10);
        e10->setReverse(e9);
        e11->setReverse(e12);
        e12->setReverse(e11);
        e13->setReverse(e14);
        e14->setReverse(e13);
        e15->setReverse(e16);
        e16->setReverse(e15);
        e17->setReverse(e18);
        e18->setReverse(e17);

        edges[7].add(e1);
        edges[7].add(e6);
        edges[1].add(e2);
        edges[1].add(e3);
        edges[2].add(e4);
        edges[2].add(e5);
        edges[2].add(e7);
        edges[3].add(e8);
        edges[3].add(e9);
        edges[4].add(e10);
        edges[4].add(e11);
        edges[5].add(e12);
        edges[5].add(e13);
        edges[6].add(e14);
        edges[6].add(e15);
        edges[0].add(e16);
        edges[0].add(e17);
        edges[3].add(e18);

        SECTION("with old edge deletion method") {
            DecompositionTree t = DecompositionTree(8, 7, edges, 3, 8, 1, false, true);
            t.updateDecomposition(0, 7);
            REQUIRE(t.getRoot()->getData()->getConnectivity() == 3);
            REQUIRE(t.getRoot()->getChildren().size() == 2);
            for (unsigned int i = 0; i < 8; i++) {
                REQUIRE(t.findSmallestSubgraph(i, i)->getData()->getConnectivity() == 7);
            }
            REQUIRE(t.findSmallestSubgraph(7, 1)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(7, 2)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(1, 2)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(7, 1)->getData()->getEndIndex() -
                    t.findSmallestSubgraph(7, 1)->getData()->getStartIndex() + 1 == 3);
            REQUIRE(t.findSmallestSubgraph(3, 7)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 1)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 2)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 4)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 5)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 6)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 0)->getData()->getConnectivity() == 6);
        }

        SECTION("with new filter-based edge deletion, core only") {
            DecompositionTree t = DecompositionTree(8, 7, edges, 3, 8, 1, false, true,
                                                    decomposition_tree::DecompositionTree::CORE_ONLY_FILTER);
            t.updateDecomposition(0, 7);
            REQUIRE(t.getRoot()->getData()->getConnectivity() == 3);
            REQUIRE(t.getRoot()->getChildren().size() == 2);
            for (unsigned int i = 0; i < 8; i++) {
                REQUIRE(t.findSmallestSubgraph(i, i)->getData()->getConnectivity() == 7);
            }
            REQUIRE(t.findSmallestSubgraph(7, 1)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(7, 2)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(1, 2)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(7, 1)->getData()->getEndIndex() -
                    t.findSmallestSubgraph(7, 1)->getData()->getStartIndex() + 1 == 3);
            REQUIRE(t.findSmallestSubgraph(3, 7)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 1)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 2)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 4)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 5)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 6)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 0)->getData()->getConnectivity() == 6);
        }

        SECTION("with new filter-based edge deletion, core and halo") {
            DecompositionTree t = DecompositionTree(8, 7, edges, 3, 8, 1, false, true,
                                                    decomposition_tree::DecompositionTree::CORE_HALO_FILTER);
            t.updateDecomposition(0, 7);
            REQUIRE(t.getRoot()->getData()->getConnectivity() == 3);
            REQUIRE(t.getRoot()->getChildren().size() == 2);
            for (unsigned int i = 0; i < 8; i++) {
                REQUIRE(t.findSmallestSubgraph(i, i)->getData()->getConnectivity() == 7);
            }
            REQUIRE(t.findSmallestSubgraph(7, 1)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(7, 2)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(1, 2)->getData()->getConnectivity() == 4);
            REQUIRE(t.findSmallestSubgraph(7, 1)->getData()->getEndIndex() -
                    t.findSmallestSubgraph(7, 1)->getData()->getStartIndex() + 1 == 3);
            REQUIRE(t.findSmallestSubgraph(3, 7)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 1)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 2)->getData()->getConnectivity() == 3);
            REQUIRE(t.findSmallestSubgraph(3, 4)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 5)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 6)->getData()->getConnectivity() == 6);
            REQUIRE(t.findSmallestSubgraph(3, 0)->getData()->getConnectivity() == 6);
        }
    }


    TEST_CASE("deletionMethodsTest", "[DecompositionTreeTest]") {
        auto t = std::unique_ptr<DecompositionTree>();
        decomposition_tree::STEP_ACTION stepAction;
        long ms;
        SECTION("core deletions") {
            SECTION("deletion method OLD") {
                t = std::make_unique<DecompositionTree>(4, 2, 2.1, 2, 2, true, true, DecompositionTree::OLD,
                                                        DecompositionTree::NAIVE, false);
            }
            SECTION("deletion method CORE_ONLY") {
                t = std::make_unique<DecompositionTree>(4, 2, 2.1, 2, 2, true, true,
                                                        DecompositionTree::CORE_ONLY_FILTER,
                                                        DecompositionTree::NAIVE, false);
            }
            t->insertAndUpdate(0, 1, stepAction, ms);
            t->insertAndUpdate(0, 2, stepAction, ms);
            t->insertAndUpdate(2, 3, stepAction, ms);
            t->insertAndUpdate(1, 2, stepAction, ms);
            CHECK(t->getEdges()[0]->getSize() == 0);
            CHECK(t->getEdges()[1]->getSize() == 0);
            CHECK(t->getEdges()[2]->getSize() == 1);
            CHECK(t->getEdges()[3]->getSize() == 1);
            CHECK(t->getEdges()[2]->getFront()->getData()->getEnd() == 3);
            CHECK(t->getEdges()[3]->getFront()->getData()->getEnd() == 2);
        }
        SECTION("core and halo deletion via filter") {
            t = std::make_unique<DecompositionTree>(4, 2, 2.1, 2, 2, true, true, DecompositionTree::CORE_HALO_FILTER,
                                                    DecompositionTree::NAIVE, false);
            t->insertAndUpdate(0, 1, stepAction, ms);
            t->insertAndUpdate(0, 2, stepAction, ms);
            t->insertAndUpdate(2, 3, stepAction, ms);
            t->insertAndUpdate(1, 2, stepAction, ms);
            CHECK(t->getEdges()[0]->getSize() == 0);
            CHECK(t->getEdges()[1]->getSize() == 0);
            CHECK(t->getEdges()[2]->getSize() == 0);
            CHECK(t->getEdges()[3]->getSize() == 0);
        }
    }


    TEST_CASE("compGraphViaChildrenTest", "[DecompositionTreeTest]") {
        auto t = std::make_unique<DecompositionTree>(4, 2, 2.1, 2, 2, false, true, DecompositionTree::OLD,
                                                     DecompositionTree::CHILDREN, false);
        long ms;
        decomposition_tree::STEP_ACTION stepAction;
        t->insertAndUpdate(0, 2, stepAction, ms);
        t->insertAndUpdate(0, 2, stepAction, ms);
        CHECK(t->getMapping()[0] == t->getMapping()[2]);
        t->insertAndUpdate(2, 3, stepAction, ms);
        t->insertAndUpdate(2, 3, stepAction, ms);
        t->insertAndUpdate(1, 2, stepAction, ms);
        CHECK(t->getMapping()[0] == t->getMapping()[2]);

    }
}