#include "acutest.h"
#include "graph.h"
#include "vamana.h"
#include <iostream>
#include <cmath>


void test_pruning(void) {
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
	int results = gready_search(graph, graph->nodes[3], graph->nodes[6], graph->k, 10, neighbours, visited);

	// for (const auto& elem : *visited) {
	// 	TEST_ASSERT(elem != NULL);
	// }

	cout << "Node neighbours before prunning: " << graph->nodes[3]->neighbours.size() << endl;


	robust_prunning(graph, graph->nodes[3], visited, 1.6, 2);

	cout << "Node neighbours after prunning: " << graph->nodes[3]->neighbours.size() << endl;

	for (const auto& r : *neighbours) {
        cout << r->to << " with distance: " << r->distance << endl;
    }

	cout << "V had: " << visited->size() << endl;

	for (const auto& r : *neighbours)
        free(r);
	
	delete neighbours;
	
	for (const auto& r : *visited)
	{    
		free(r);
	}

	cout << "V has: " << visited->size() << endl;
	delete visited;
    
	destroy_graph(graph);
	return;
}

TEST_LIST = {
	{ "test_pruning", test_pruning },
	{ NULL, NULL }
};