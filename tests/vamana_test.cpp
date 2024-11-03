#include "acutest.h"
#include <iostream>
#include "graph.h"
#include "vamana.h"
#include "io.h"

using namespace std;

void test_create_vamana_index(void) {
    
    string path = "../data/siftsmall/siftsmall_base.fvecs";
    Graph graph;
    int L = 100;
    int R = 20;

    // TEST_ASSERT(!create_vamana_index(&graph, path, L, R));
    // TEST_ASSERT(graph != NULL);
    
    destroy_graph(graph);
}

TEST_LIST = {
    { "vamana_create", test_create_vamana_index},
    { NULL, NULL}
};