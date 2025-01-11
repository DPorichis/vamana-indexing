#include "acutest.h"
#include "graph.h"
#include "vamana.h"
#include "vamana-utils.h"

#include <iostream>
#include <cmath>


void test_pruning(void) {
	Graph graph = create_graph('f', 5, 2, true);
	int n = 30;
	for(int i = 0; i < n; i++)
	{	
		float* point = (float*)malloc(sizeof(*point)*2);
    	point[0] = i;
    	point[1] = i;
		Node item = add_node_graph(graph, 2, point, i);
	}
	init_dummy_graph(graph);


	set<Candidate, CandidateComparator>* neighbours = new set<Candidate, CandidateComparator>();
    set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();

	int results = gready_search(graph, graph->nodes[find_medoid(graph)], graph->nodes[3], graph->k, 10, neighbours, visited);

	cout << "Node neighbours before prunning: " << graph->nodes[3]->neighbours.size() << endl;

	double max_distance = 0;

	for (const auto& r : graph->nodes[3]->neighbours) {
        cout << r->to << " with distance: " << r->distance << endl;
		if (r->distance > max_distance)
			max_distance = r->distance;
    }

	robust_prunning(graph, graph->nodes[3], visited, 3, 2);

	// Good reason for termination
	TEST_ASSERT(graph->nodes[3]->neighbours.size() ==  2 || visited->size() == 0);

	cout << "Node neighbours after prunning: " << graph->nodes[3]->neighbours.size() << endl;

	for (const auto& r : graph->nodes[3]->neighbours) {
        cout << r->to << " with distance: " << r->distance << endl;
		// Check if we end up with closer neighbours than we started
		TEST_ASSERT(max_distance >= r->distance);
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
	return;
}

TEST_LIST = {
	{ "test_pruning", test_pruning },
	{ NULL, NULL }
};