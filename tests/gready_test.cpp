#include "acutest.h"
#include "graph.h"
#include "vamana.h"
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
		TEST_ASSERT(item->neighbours.size() > ceil(log2(n)));
	}

    destroy_graph(graph);
	return;
}

TEST_LIST = {
	{ "init_dummy_graph", test_dummy},
	{ NULL, NULL }
};
