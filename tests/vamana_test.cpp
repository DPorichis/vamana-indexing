#include "acutest.h"
#include <iostream>
#include <algorithm>
#include <set>
#include <math.h>
#include "graph.h"
#include "vamana.h"
#include "vamana-utils.h"

#include "io.h"

using namespace std;

void test_create_vamana_index(void) {
    
    int dimensions = 100;

    // Dataset
    string path = "../data/dummy-data.bin";
    string queries = "../data/dummy-queries.bin";

    // Groundtruth data
    string groundtruth_file = "../data/groundtruth.bin";
    vector<vector<uint32_t>> groundtruth;
    create_groundtruth_file(path, queries, groundtruth_file);
    readKNN(groundtruth_file, dimensions, groundtruth);

    Graph graph;
    int L = 80;
    int R = 20;
    int K = 70;
    int a = 1.2;

    int medoid_pos;
    TEST_ASSERT(!create_vamana_index(&graph, path, L, R, a, medoid_pos, dimensions));

    
    set<Candidate, CandidateComparator>* neighbors = new set<Candidate, CandidateComparator>();
    set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();

    
    int pos, query_type = 1;
    Node query;
    while (query_type != 0) {
        query = ask_query(queries, query_type, graph->dimensions, pos);
    }

	// Node query = ask_query(queries, query_type, graph->dimensions, pos);
    gready_search(graph, graph->nodes[graph->unfiltered_medoid], query, K, L, neighbors, visited);
    int i = 0;
    // Print the nodes
    // for (const auto& r : *neighbours) {
    //     cout << r->to->pos << " with distance: " << r->distance << endl;
    //     cout << "Must be : " << vectors[pos].components[i] << endl;
    //     i++;
    // }
    
    // Recall calculation
    set<int> algorithm_results;
    for (const auto& r : *neighbors) {
        if (K == i)
            break;
        algorithm_results.insert(r->to->pos);
        i++;
    }
    set<int> true_results;

    for(int i = 0; i < 100; i++)
        true_results.insert(groundtruth[pos][i]); 
    
    set<int> intersection;
    set_intersection(algorithm_results.begin(), algorithm_results.end(),
                     true_results.begin(), true_results.end(),
                     inserter(intersection, intersection.begin()));

    int j = 0;
    for (const auto& r : *neighbors) {
        cout << "Node: " << r->to->pos << " with distance: " << r->distance << endl;
        float sum = 0.0;
        // Skip the first 2 dimensions
        for (size_t i = 0; i < 100; ++i) {
            float diff = ((float*)graph->nodes[groundtruth[pos][j]]->components)[i] - ((float*)query->components)[i];
            sum += diff * diff;
        }
        cout << "Correct Node: " << groundtruth[pos][j] << " with distance: " << sqrt(sum) << endl;
        j++;
    }

    double recall = static_cast<double>(intersection.size()) / true_results.size();
    cout << "Recall: " << recall * 100 << "%" << endl;

    cout << "Query with position: " << pos << endl;
    cout << "##########################" << endl << endl;    


    for (const auto& r : *neighbors)
        free(r);
	
	delete neighbors;
	
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