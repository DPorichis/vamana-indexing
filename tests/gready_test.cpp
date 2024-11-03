#include "acutest.h"
#include "graph.h"
#include "vamana.h"
#include "io.h"
#include <iostream>
#include <cmath>

using namespace std;

void test_dummy(void) {
	Graph graph = create_graph('f', 5, 2);
	int n = 30;
	for(int i = 0; i < n; i++)
	{	
		float* point = (float*)malloc(sizeof(*point)*2);
    	point[0] = i;
    	point[1] = i;
		Node item = add_node_graph(graph, 2, point);
		
	}
	init_dummy_graph(graph);
	int total_num = 0;
	for(int i = 0; i < n; i++)
	{
		Node item = graph->nodes[i];
		// In order for the graph to be connected (according to the paper)
		// cout << item->neighbours.size() << " > " << ceil(log2(n)) << endl;
		TEST_ASSERT(item->neighbours.size() > ceil(log2(n)));
	}

    destroy_graph(graph);
	return;
}

void test_gready_search(void) {
	Graph graph = create_graph('f', 5, 2);
	int n = 30;
	for(int i = 0; i < n; i++)
	{	
		float* point = (float*)malloc(sizeof(*point)*2);
    	point[0] = i;
    	point[1] = i;
		Node item = add_node_graph(graph, 2, point);
	}
	init_dummy_graph(graph);

	set<Candidate, CandidateComparator>* neighbours = new set<Candidate, CandidateComparator>();
    set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();

	int results = gready_search(graph, find_medoid(graph), graph->nodes[6], graph->k, 10, neighbours, visited);

	int flag = 0;
	for (const auto& r : *neighbours) {
        cout << r->to << " with distance: " << r->distance << endl;
		if(r->distance < 7.1)
			flag++;
    }

	TEST_ASSERT(neighbours->size() == 10);
	TEST_ASSERT(flag >= 7); // Has more than 70% accuracy


	for (const auto& r : *neighbours)
        free(r);
	
	delete neighbours;
	
	for (const auto& r : *visited)
        free(r);
	
	delete visited;
    
	destroy_graph(graph);
	return;
}

void test_medoid(void) {
	Graph graph = create_graph('f', 5, 2);
	int n = 31;
	for(int i = 0; i < n; i++)
	{	
		float* point = (float*)malloc(sizeof(*point)*2);
    	point[0] = i;
    	point[1] = i;
		Node item = add_node_graph(graph, 2, point);
	}

	Node medoid = find_medoid(graph);

	cout << "Medoid Found " << medoid << endl << "[";
	float* point = (float*)medoid->components;
	for(int i = 0; i < medoid->d_count; i++)
		cout << ", " << point[i];
	cout << "]" << endl;

	// Test if it finds the real medoid
	TEST_ASSERT(point[0] == 15);
	TEST_ASSERT(point[0] == 15);

	destroy_graph(graph);

	string path = "../data/siftsmall/siftsmall_base.fvecs";
    int L = 100;
    int R = 20;
    graph = create_graph_from_file(path, 'f', 5);
    TEST_ASSERT(graph->dimensions == 128);
    Node node = find_medoid(graph);
    TEST_ASSERT(node->d_count == 128);


	return;
}

TEST_LIST = {
	{ "init_dummy_graph", test_dummy},
	{ "test_medoid", test_medoid },
	{ "gready_search", test_gready_search},
	{ NULL, NULL }
};
