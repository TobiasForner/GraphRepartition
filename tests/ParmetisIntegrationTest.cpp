//
// Created by tobias on 16.12.19.
//

#ifdef METIS_ENABLED

#include "catch.h"
#include "../src/ParMetisConnectedComponentComputation.h"

//all the tests are in this one case because MPI can only be initialized once
TEST_CASE("convertToMetisGraphTest", "[ParmetisIntegrationTest]") {
    decomposition_tree::Graph g(5);
    g.addEdge(decomposition_tree::Edge(0, 1, 1));
    g.addEdge(decomposition_tree::Edge(1, 2, 1));
    g.addEdge(decomposition_tree::Edge(2, 3, 1));
    g.addEdge(decomposition_tree::Edge(3, 4, 1));
    g.addEdge(decomposition_tree::Edge(4, 0, 1));

    metis_graph res = ParMetisConnectedComponentComputation::convertToMetisGraph(g);
    REQUIRE(res.xadj[0] == 0);
    REQUIRE(res.xadj[1] == 2);
    REQUIRE(res.xadj[2] == 4);
    REQUIRE(res.xadj[3] == 6);
    REQUIRE(res.xadj[4] == 8);
    REQUIRE(res.xadj[5] == 10);

    //order may need to be handled more leniently
    CHECK(res.adjncy[0] == 1);
    CHECK(res.adjncy[1] == 4);
    CHECK(res.adjncy[2] == 0);
    CHECK(res.adjncy[3] == 2);
    CHECK(res.adjncy[4] == 1);
    CHECK(res.adjncy[5] == 3);
    CHECK(res.adjncy[6] == 2);
    CHECK(res.adjncy[7] == 4);
    CHECK(res.adjncy[8] == 3);
    CHECK(res.adjncy[9] == 0);
    for (unsigned int i = 0; i < g.getEdgesNr(); i++) {
        CHECK(res.adjwgt[i] == 1);
    }
    delete[]res.xadj;
    delete[]res.adjwgt;
    delete[]res.adjncy;

}

TEST_CASE("testInsertAndUpdateWorks", "[ParmetisIntegrationTest]") {
    auto g = decomposition_tree::Graph(6);
    auto p = ParMetisConnectedComponentComputation(g, 5, 3, 2, 2.1, true, true);
    g.addEdge(decomposition_tree::Edge(0, 1, 1));
    g.addEdge(decomposition_tree::Edge(1, 2, 1));
    g.addEdge(decomposition_tree::Edge(2, 3, 1));
    g.addEdge(decomposition_tree::Edge(3, 4, 1));
    g.addEdge(decomposition_tree::Edge(4, 0, 1));
    decomposition_tree::STEP_ACTION stepAction;
    long ms;
    p.insertAndUpdate(0, 1, stepAction, ms);
    p.insertAndUpdate(1, 2, stepAction, ms);
    p.insertAndUpdate(2, 3, stepAction, ms);
    p.insertAndUpdate(3, 4, stepAction, ms);
    p.insertAndUpdate(4, 0, stepAction, ms);
    p.insertAndUpdate(2, 4, stepAction, ms);
}

#endif