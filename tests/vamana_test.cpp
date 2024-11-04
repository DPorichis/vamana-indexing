#include "acutest.h"
#include <iostream>
#include "graph.h"
#include "vamana.h"
#include "io.h"

using namespace std;

void test_create_vamana_index(void) {
    
    string path = "../data/siftsmall/siftsmall_base.fvecs";

    // Groundtruth data
    string groundtruth = "../data/siftsmall/siftsmall_groundtruth.ivecs";
    vector<file_vector2> vectors = read_int_vectors_from_file(groundtruth);

    Graph graph;
    int L = 20;
    int R = 5;
    int K = 10;
    graph = create_graph_from_file(path, 'f', K);
    TEST_ASSERT(!create_vamana_index(&graph, path, L, R));
    // TEST_ASSERT(graph != NULL);
    
    set<Candidate, CandidateComparator>* neighbours = new set<Candidate, CandidateComparator>();
    set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();

    string queries = "../data/siftsmall/siftsmall_query.fvecs";
    int pos; 
	Node query = ask_query(queries, graph->dimensions, pos);
    gready_search(graph, graph->nodes[8736], query, K, L, neighbours, visited);
    int i = 0;
    for (const auto& r : *neighbours) {
        cout << r->to->pos << " with distance: " << r->distance << endl;
        cout << "Must be : " << vectors[pos].components[i] << endl;
        i++;
    }


    destroy_graph(graph);
}

TEST_LIST = {
    { "vamana_create", test_create_vamana_index},
    { NULL, NULL}
};