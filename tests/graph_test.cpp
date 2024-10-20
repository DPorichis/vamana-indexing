#include "acutest.h"
#include "graph.h"
#include <iostream>

using namespace std;

void test_create(void) {
	Graph graph = create_graph('f', 5, 3);

	TEST_ASSERT(graph != NULL);
    TEST_ASSERT(graph->type == 'f');
    TEST_ASSERT(graph->k == 5);
    TEST_ASSERT(graph->dimensions == 3);
    TEST_ASSERT(graph->nodes.size() == 0);

	destroy_graph(graph);
}

void test_insert(void){
    Graph graph = create_graph('f', 5, 3);

    float* point = (float*)malloc(sizeof(*point)*3);
    point[0] = 0;
    point[1] = 0;
    point[2] = 0;

    Node item = add_node_graph(graph, 3, point);

    TEST_ASSERT(item!= NULL);
    TEST_ASSERT(graph->nodes.size() == 1);
    TEST_ASSERT(graph->nodes[0] == item);
    TEST_ASSERT(item->neighbours.empty());
    TEST_ASSERT(item->n_count == 0);
    TEST_ASSERT(item->components == point);
    
    item = add_node_graph(graph, 6, point);
    TEST_ASSERT(item== NULL);
    TEST_ASSERT(graph->nodes.size() == 1);

    float* point1 = (float*)malloc(sizeof(*point1)*3);
    point1[0] = 1;
    point1[1] = 1;
    point1[2] = 1;

    item = add_node_graph(graph, 3, point1);

    TEST_ASSERT(point!= NULL);
    TEST_ASSERT(graph->nodes.size() == 2);
    TEST_ASSERT(graph->nodes[1] == item);
    TEST_ASSERT(item->neighbours.empty());
    TEST_ASSERT(item->n_count == 0);
    TEST_ASSERT(item->components == point1);


    destroy_graph(graph);
}

void test_neighbours(void){
    Graph graph = create_graph('f', 5, 3);

    float* point0 = (float*)malloc(sizeof(*point0)*3);
    point0[0] = 0;
    point0[1] = 0;
    point0[2] = 0;

    Node item0 = add_node_graph(graph, 3, point0);

    float* point1 = (float*)malloc(sizeof(*point1)*3);
    point1[0] = 1;
    point1[1] = 1;
    point1[2] = 1;

    Node item1 = add_node_graph(graph, 3, point1);
    
    float dist = add_neighbour_node(item0, item1);
    TEST_ASSERT(dist > 0);
    TEST_ASSERT(item0->neighbours.size() == 1);
    TEST_ASSERT(item1->neighbours.empty());

    // Doesn't allow self to self
    dist = add_neighbour_node(item0, item0);
    TEST_ASSERT(dist < 0);

    // Doesn't allow double instances of the same neighbour
    dist = add_neighbour_node(item0, item1);
    TEST_ASSERT(dist < 0);
    TEST_ASSERT(item0->neighbours.size() == 1);

    destroy_graph(graph);

}

TEST_LIST = {
	{ "create_graph", test_create},
    { "add_node_graph", test_insert},
    { "create_neighbours", test_neighbours},

	{ NULL, NULL }
};
