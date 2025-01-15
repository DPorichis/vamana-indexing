#include "acutest.h"
#include <iostream>
#include <algorithm>
#include <set>
#include <math.h>
#include "graph.h"
#include "vamana.h"
#include "filtered-vamana.h"
#include "vamana-utils.h"

#include "io.h"

using namespace std;

void test_create_vamana_index(void) {
    
    int dimensions = 100;

    // Dataset
    string path = "./data/test-data.bin";
    string queries_path = "./data/test-queries.bin";

    // Groundtruth data
    string groundtruth_file = "./data/test-groundtruth.bin";
    vector<vector<uint32_t>> groundtruth;
    readKNN(groundtruth_file, dimensions, groundtruth);

    Graph graph;
    int L = 50;
    int R = 40;
    int K = 100;
    int a = 1.2;

    int medoid_pos;
    TEST_ASSERT(!create_vamana_index(&graph, path, L, R, a, medoid_pos, dimensions, 'n', 0, false));
    
    set<Candidate, CandidateComparator>* neighbors = new set<Candidate, CandidateComparator>();
    set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();
    
    int query_type = 1;
    vector<vector<float>> queries;
	readBinary(queries_path, dimensions + 4, queries);

    int pos;
	srand(static_cast<unsigned int>(time(0)));
	pos = rand() % queries.size();
    Node query = ask_query(query_type, graph->dimensions, pos, queries);

    int j = pos;
    while (query_type != 0) {
        destroy_node(query);
        query = ask_query(query_type, graph->dimensions, j, queries);
        j = (j + 1) % queries.size();
    }

	// Node query = ask_query(queries, query_type, graph->dimensions, pos);
    gready_search(graph, graph->nodes[graph->unfiltered_medoid], query, K, L, neighbors, visited);
    int i = 0;
    
    // Recall calculation
    set<int> algorithm_results;
    for (const auto& r : *neighbors) {
        if (K == i)
            break;
        algorithm_results.insert(r->to->pos);
        i++;
    }
    
    j = 0;
    for (const auto& r : *neighbors) {
        cout << "Node: " << r->to->pos << " with distance: " << r->distance << endl;
    }

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

void test_create_filtered_vamana_index(void) {
    
    int dimensions = 100;

    // Dataset
    string path = "./data/test-data.bin";
    string queries_path = "./data/test-queries.bin";

    // Groundtruth data
    string groundtruth_file = "./data/test-groundtruth.bin";    
    vector<vector<uint32_t>> groundtruth;
    readKNN(groundtruth_file, dimensions, groundtruth);

    Graph graph;
    int L = 50;
    int R = 40;
    int K = 100;
    int a = 1.2;

    int medoid_pos;
    TEST_ASSERT(!create_filtered_vamana_index(&graph, path, L, R, a, dimensions, false, false));
    TEST_ASSERT(!graph->all_categories.empty());

    set<Candidate, CandidateComparator>* neighbors = new set<Candidate, CandidateComparator>();
    set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();

    int query_type = 1;
    vector<vector<float>> queries;
	readBinary(queries_path, dimensions + 4, queries);

    int pos;
	srand(static_cast<unsigned int>(time(0)));
	pos = rand() % queries.size();
    Node query = ask_query(query_type, graph->dimensions, pos, queries);
    
    int j = pos;
    while(query_type == 2 || query_type == 3)
    {
        destroy_node(query);
        query = ask_query(query_type, graph->dimensions, j, queries);
        j = (j + 1) % queries.size();
    }

    set<int> categories;
    if(query_type == 0)
    {
        categories.clear();
        categories.insert(graph->all_categories.begin(), graph->all_categories.end());
    }
    else if(query_type == 1)
    {
        categories.insert(query->category);
    }
    int s_count = categories.size();
    Node* S = (Node *)malloc(sizeof(*S)*s_count);
    j = 0;
    for (const int& val : categories) {
        S[j] = graph->nodes[graph->medoid_mapping[val]];
        j++;
    }

    filtered_gready_search(graph, S, s_count, query, K, L, categories, neighbors, visited);

    int i = 0;
    // Print the nodes
    // for (const auto& r : *neighbours) {
    //     cout << r->to->pos << " with distance: " << r->distance << endl;
    //     cout << "Must be : " << vectors[pos].components[i] << endl;
    //     i++;
    // }
    
    j = 0;
    for (const auto& r : *neighbors) {
        cout << "Node: " << r->to->pos << " with distance: " << r->distance << endl;
    }

    cout << "Query with position: " << pos << endl;
    cout << "##########################" << endl << endl;    


    for (const auto& r : *neighbors)
        free(r);
	
	delete neighbors;
	
	for (const auto& r : *visited)
        free(r);
	
	delete visited;

    free(S);

    destroy_node(query);
    destroy_graph(graph);
}

void test_create_filtered_vamana_index_parallel(void) {
    
    int dimensions = 100;

    // Dataset
    string path = "./data/test-data.bin";
    string queries_path = "./data/test-queries.bin";

    // Groundtruth data
    string groundtruth_file = "./data/test-groundtruth.bin";    
    vector<vector<uint32_t>> groundtruth;
    readKNN(groundtruth_file, dimensions, groundtruth);

    Graph graph;
    int L = 50;
    int R = 40;
    int K = 100;
    int a = 1.2;

    int medoid_pos;
    TEST_ASSERT(!create_filtered_vamana_index_parallel(&graph, path, L, R, a, dimensions, 4));
    TEST_ASSERT(!graph->all_categories.empty());

    set<Candidate, CandidateComparator>* neighbors = new set<Candidate, CandidateComparator>();
    set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();

    int query_type = 1;
    vector<vector<float>> queries;
	readBinary(queries_path, dimensions + 4, queries);

    int pos;
	srand(static_cast<unsigned int>(time(0)));
	pos = rand() % queries.size();
    Node query = ask_query(query_type, graph->dimensions, pos, queries);
    
    int j = pos;
    while(query_type == 2 || query_type == 3)
    {
        destroy_node(query);
        query = ask_query(query_type, graph->dimensions, j, queries);
        j = (j + 1) % queries.size();
    }

    set<int> categories;
    if(query_type == 0)
    {
        categories.clear();
        categories.insert(graph->all_categories.begin(), graph->all_categories.end());
    }
    else if(query_type == 1)
    {
        categories.insert(query->category);
    }
    int s_count = categories.size();
    Node* S = (Node *)malloc(sizeof(*S)*s_count);
    j = 0;
    for (const int& val : categories) {
        S[j] = graph->nodes[graph->medoid_mapping[val]];
        j++;
    }

    filtered_gready_search(graph, S, s_count, query, K, L, categories, neighbors, visited);

    int i = 0;
    // Print the nodes
    // for (const auto& r : *neighbours) {
    //     cout << r->to->pos << " with distance: " << r->distance << endl;
    //     cout << "Must be : " << vectors[pos].components[i] << endl;
    //     i++;
    // }
    
    for (const auto& r : *neighbors) {
        cout << "Node: " << r->to->pos << " with distance: " << r->distance << endl;
    }
    cout << "Query with position: " << pos << endl;
    cout << "##########################" << endl << endl;    


    for (const auto& r : *neighbors)
        free(r);
	
	delete neighbors;
	
	for (const auto& r : *visited)
        free(r);
	
	delete visited;

    free(S);

    destroy_node(query);
    destroy_graph(graph);
}

void test_create_stiched_vamana_index(void) {
    
    int dimensions = 100;

    // Dataset
    string path = "./data/test-data.bin";
    string queries_path = "./data/test-queries.bin";

    // Groundtruth data
    string groundtruth_file = "./data/test-groundtruth.bin";
    vector<vector<uint32_t>> groundtruth;
    readKNN(groundtruth_file, dimensions, groundtruth);

    int L = 80;
    int R = 20;
    int K = 70;
    int a = 1.2;

    int medoid_pos;
    
    Graph index_mapping = create_stiched_vamana_index(path, 'f', L, R, R, a, dimensions, false, 0, false);
    
    TEST_ASSERT(index_mapping != NULL);

    cout << "Index is ready" << endl;
    cout << "Ground truth is ready" << endl;

    srand(static_cast<unsigned int>(time(0)));

    int query_type = 1;
    vector<vector<float>> queries;
	readBinary(queries_path, dimensions + 4, queries);

    int pos;
	pos = rand() % queries.size();
    Node query = ask_query(query_type, index_mapping->dimensions, pos, queries);
    
    int j = pos;
    while(query_type == 2 || query_type == 3)
    {
        destroy_node(query);
        query = ask_query(query_type, index_mapping->dimensions, j, queries);
        j = (j + 1) % queries.size();
    }
    
    set<Candidate, CandidateComparator>* total_neighbors = new set<Candidate, CandidateComparator>();
    
    //cout << "Performing Query #" << query_pos << " of type " << query_type << endl;

    set<int> categories;
    if(query_type == 0)
    {
        categories.clear();
        for (const auto& val : index_mapping->all_categories)
            categories.insert(val);
    }
    else if(query_type == 1)
    {
        categories.insert(query->category);
    }

    for (const int& val : categories) {
        
        set<Candidate, CandidateComparator>* neighbors = new set<Candidate, CandidateComparator>();
        set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();
    
        gready_search(index_mapping, index_mapping->nodes[index_mapping->unfiltered_medoid], query, K, L, neighbors, visited);
        
        for (auto it = neighbors->begin(); it != neighbors->end(); ++it) {
            Candidate to_insert = create_candidate_copy(*it);
            auto result = total_neighbors->insert(to_insert);
            if (!result.second) {
                free(to_insert);
            }
        }

        for (const auto& r : *neighbors)
            free(r);
        delete neighbors;
        
        for (const auto& r : *visited)
            free(r);
        delete visited;
    }
    // Results
    set<int> algorithm_results;
    j = 0;
    for (const auto& r : *total_neighbors) {
        if (K == j)
            break;
        algorithm_results.insert(r->to->pos);
        j++;
    }

    j = 0;
    for (const auto& r : *total_neighbors) {
        if(K == j)
            break;
        cout << "Node: " << r->to->pos << " with distance: " << r->distance << endl;
        j++;
    }
    cout << "Query with position: " << query->pos << endl;
    cout << "##########################" << endl << endl;    
    
    
    for (const auto& r : *total_neighbors)
        free(r);
    
    delete total_neighbors;
    
    destroy_node(query);
    destroy_graph(index_mapping);
}

void test_parallel_stitched_vamana_index(void) {
    
    int dimensions = 100;

    // Dataset
    string path = "./data/test-data.bin";
    string queries_path = "./data/test-queries.bin";

    // Groundtruth data
    string groundtruth_file = "./data/test-groundtruth.bin";
    vector<vector<uint32_t>> groundtruth;
    readKNN(groundtruth_file, dimensions, groundtruth);

    int L = 50;
    int R = 40;
    int K = 100;
    int a = 1.2;

    int medoid_pos;
    
    Graph index_mapping = create_stiched_vamana_index_parallel(path, 'f', L, R, R, a, dimensions, 4, 0, false);
    
    TEST_ASSERT(index_mapping != NULL);

    cout << "Index is ready" << endl;
    cout << "Ground truth is ready" << endl;

    srand(static_cast<unsigned int>(time(0)));

    int query_type = 1;
    vector<vector<float>> queries;
	readBinary(queries_path, dimensions + 4, queries);

    int pos;
	pos = rand() % queries.size();
    Node query = ask_query(query_type, index_mapping->dimensions, pos, queries);
    
    int j = pos;
    while(query_type == 2 || query_type == 3)
    {
        destroy_node(query);
        query = ask_query(query_type, index_mapping->dimensions, j, queries);
        j = (j + 1) % queries.size();
    }
    
    set<Candidate, CandidateComparator>* total_neighbors = new set<Candidate, CandidateComparator>();
    
    //cout << "Performing Query #" << query_pos << " of type " << query_type << endl;

    set<int> categories;
    if(query_type == 0)
    {
        categories.clear();
        for (const auto& val : index_mapping->all_categories)
            categories.insert(val);
    }
    else if(query_type == 1)
    {
        categories.insert(query->category);
    }

    for (const int& val : categories) {
        
        set<Candidate, CandidateComparator>* neighbors = new set<Candidate, CandidateComparator>();
        set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();
    
        gready_search(index_mapping, index_mapping->nodes[index_mapping->unfiltered_medoid], query, K, L, neighbors, visited);
        
        for (auto it = neighbors->begin(); it != neighbors->end(); ++it) {
            Candidate to_insert = create_candidate_copy(*it);
            auto result = total_neighbors->insert(to_insert);
            if (!result.second) {
                free(to_insert);
            }
        }

        for (const auto& r : *neighbors)
            free(r);
        delete neighbors;
        
        for (const auto& r : *visited)
            free(r);
        delete visited;
    }
    // Results
    set<int> algorithm_results;
    j = 0;
    for (const auto& r : *total_neighbors) {
        if (K == j)
            break;
        algorithm_results.insert(r->to->pos);
        j++;
    }

    j = 0;
    for (const auto& r : *total_neighbors) {
        if(K == j)
            break;
        cout << "Node: " << r->to->pos << " with distance: " << r->distance << endl;
        j++;
    }
    cout << "Query with position: " << query->pos << endl;
    cout << "##########################" << endl << endl;    
    
    
    for (const auto& r : *total_neighbors)
        free(r);
    
    delete total_neighbors;
    
    destroy_node(query);
    destroy_graph(index_mapping);
}

TEST_LIST = {
    { "vamana_index", test_create_vamana_index},
    { "filtered_vamana_create", test_create_filtered_vamana_index},
    { "parallel_filtered_vamana_create", test_create_filtered_vamana_index_parallel},
    { "stiched_vamana_create", test_create_stiched_vamana_index},
    { "parallel_stitched_vamana_create", test_parallel_stitched_vamana_index},
    { NULL, NULL}
};