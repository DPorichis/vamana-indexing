#include "acutest.h"
#include "graph.h"
#include "filtered-vamana.h"
#include "vamana.h"
#include "vamana-utils.h"

#include "io.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <set>

using namespace std;

void test_gready_search(void) {
	Graph graph = create_graph('f', 5, 2, false);
	int n = 30;
	for(int i = 0; i < n; i++)
	{	
		float* point = (float*)malloc(sizeof(*point)*2);
    	point[0] = i;
    	point[1] = i;
		Node item = add_node_graph(graph, 2, point, i, i%5);
	}
	init_dummy_graph(graph, 0);

	set<Candidate, CandidateComparator>* neighbours = new set<Candidate, CandidateComparator>();
    set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();

    set<int> all_categories;
    for(int i = 0; i < 5; i++)
        all_categories.insert(i);

    set<int> query_categories = {2,3};

    map<int, int> medoid_map;
    int medoid = find_filtered_medoid(graph, all_categories, &medoid_map);
    
    int s_count = 2;

    Node* S = (Node *)malloc(sizeof(*S)*s_count);

    int i = 0;
    for (const int& val : query_categories) {
        cout << val << " -> " << graph->nodes[medoid_map[val]] << endl;
        S[i] = graph->nodes[medoid_map[val]];
        i++;
    }

    int results = filtered_gready_search(graph, S, s_count, graph->nodes[6], graph->k, 10, query_categories, neighbours, visited);
	
	int flag = 0;
	for (const auto& r : *neighbours) {
        cout << r->to << "with distance: " << r->distance << endl;
        cout << "Category: " << r->to->category << endl;

        set<int>::iterator itr;   
        if(r->to->category != 2 && r->to->category != 3)
        {
            cout << r->to << "Doesn't have the right category " << endl;
            flag++;
        }
    }
    
    TEST_ASSERT(flag <= 0); // Returns only neighbours of correct categories


	for (const auto& r : *neighbours)
        free(r);
	
	delete neighbours;
	
	for (const auto& r : *visited)
        free(r);
	
	delete visited;
    
	destroy_graph(graph);
    free(S);
	return;
}

void test_medoid(void) {
	Graph graph = create_graph('f', 5, 2, false);
	int n = 30;
    set<int> all_categories;
	for(int i = 0; i < n; i++)
	{	
		float* point = (float*)malloc(sizeof(*point)*2);
    	set<int> categories;
        point[0] = i;
    	point[1] = i;
		Node item = add_node_graph(graph, 2, point, i, i%5);
	}

    for(int i = 0; i < 5; i++)
        all_categories.insert(i);

	// Simple test

    map<int,int> medoid_map;
	int medoid = find_filtered_medoid(graph, all_categories, &medoid_map);
    
    // Check that medoid map has succesfully copied to graph
    TEST_ASSERT(medoid_map == graph->medoid_mapping);

    for(int i = 0; i < 5; i++)
    {
        TEST_ASSERT(medoid_map[i]%5 == i);
    }
	destroy_graph(graph);
	return;
}

void test_pruning(void) {
	Graph graph = create_graph('f', 5, 2, false);
	int n = 30;
	for(int i = 0; i < n; i++)
	{
		float* point = (float*)malloc(sizeof(*point)*2);
    	point[0] = i;
    	point[1] = i;
		Node item = add_node_graph(graph, 2, point, i, i%5);	
    }
	init_dummy_graph(graph, 0);

    set<int> all_categories;
    for(int i = 0; i < 5; i++)
        all_categories.insert(i);

    set<int> query_categories = {2};

    map<int, int> medoid_map;
    int medoid = find_filtered_medoid(graph, all_categories, &medoid_map);

    int s_count = 1;

    Node* S = (Node *)malloc(sizeof(*S)*s_count);

    int i = 0;
    for (const int& val : query_categories) {
        cout << val << " -> " << graph->nodes[medoid_map[val]] << endl;
        S[i] = graph->nodes[medoid_map[val]];
        i++;
    }

	set<Candidate, CandidateComparator>* neighbours = new set<Candidate, CandidateComparator>();
    set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();
	int results = filtered_gready_search(graph, S, s_count, graph->nodes[7], graph->k, 10, query_categories, neighbours, visited);

	cout << "Node neighbours before prunning: " << graph->nodes[7]->neighbours.size() << endl;

    double max_dist = 0;

    for (const auto& r : graph->nodes[7]->neighbours) {
        cout << r->to << " with distance: " << r->distance << endl;
        // Check that neighbours are of correct category or they pass the proximity test
        if(r->distance > max_dist)
            max_dist = r->distance;
    }

	set<Link, LinkComp> old = graph->nodes[7]->neighbours;

	filtered_robust_prunning(graph, graph->nodes[7], visited, 1.2, 2);

	// Good reason for termination
	TEST_ASSERT(graph->nodes[7]->neighbours.size() ==  2 || visited->size() == 0);

	cout << "Node neighbours after prunning: " << graph->nodes[7]->neighbours.size() << endl;

	for (const auto& r : graph->nodes[7]->neighbours) {
        cout << r->to << " with distance: " << r->distance << endl;
        // Check that neighbours are of correct category or they pass the proximity test

        TEST_ASSERT(r->to->category == 2 || max_dist >= r->distance);

    }
    
	cout << "V had: " << visited->size() << endl;

	for (const auto& r : *neighbours)
        free(r);
	
	delete neighbours;
	
	for (const auto& r : *visited)
	{    
		free(r);
	}
	delete visited;

	destroy_graph(graph);
	free(S);
    
    return;
}

TEST_LIST = {
	{ "test_medoid", test_medoid },
    { "test_gready", test_gready_search },
    { "test_pruning", test_pruning },
	{ NULL, NULL }
};
