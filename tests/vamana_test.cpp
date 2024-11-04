#include "acutest.h"
#include <iostream>
#include <algorithm>
#include <set>
#include "graph.h"
#include "vamana.h"
#include "io.h"

using namespace std;

void test_create_vamana_index(void) {
    
    string path = "../data/siftsmall/siftsmall_base.fvecs";

    // Groundtruth data
    string groundtruth = "../data/siftsmall/siftsmall_groundtruth.ivecs";
    vector<file_vector_int> vectors = read_int_vectors_from_file(groundtruth);

    Graph graph;
    int L = 80;
    int R = 20;
    int K = 70;
    int a = 1.2;
    //    graph = create_graph_from_file(path, 'f', K);
    int medoid_pos;
    TEST_ASSERT(!create_vamana_index(&graph, path, L, R, a, medoid_pos));
    // TEST_ASSERT(graph != NULL);
    
    set<Candidate, CandidateComparator>* neighbours = new set<Candidate, CandidateComparator>();
    set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();

    string queries = "../data/siftsmall/siftsmall_query.fvecs";
    int pos; 
	Node query = ask_query(queries, graph->type, graph->dimensions, pos);
    gready_search(graph, graph->nodes[medoid_pos], query, K, L, neighbours, visited);
    int i = 0;
    // Print the nodes
    // for (const auto& r : *neighbours) {
    //     cout << r->to->pos << " with distance: " << r->distance << endl;
    //     cout << "Must be : " << vectors[pos].components[i] << endl;
    //     i++;
    // }
    
    // Recall calculation
    i = 0;
    set<int> algorithm_results;
    for (const auto& r : *neighbours) {
        if (K == i)
            break;
        algorithm_results.insert(r->to->pos);
        i++;
    }
    set<int> true_results(vectors[pos].components.begin(), vectors[pos].components.begin() + K);
    
    set<int> intersection;
    set_intersection(algorithm_results.begin(), algorithm_results.end(),
                     true_results.begin(), true_results.end(),
                     inserter(intersection, intersection.begin()));

    double recall = static_cast<double>(intersection.size()) / true_results.size();
    cout << "Recall: " << recall * 100 << "%" << endl;

    for (const auto& r : *neighbours)
        free(r);
	
	delete neighbours;
	
	for (const auto& r : *visited)
        free(r);
	
	delete visited;
        
    destroy_node(query);
    destroy_graph(graph);
}

TEST_LIST = {
    { "vamana_create", test_create_vamana_index},
    { NULL, NULL}
};