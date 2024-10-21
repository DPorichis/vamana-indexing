// This file contains the graph structure implementation and relevant functions
#include "header.h"
#include "graph.h"
#include <iostream>
#include <cmath>

using namespace std;

// Creates a link from-to
Link create_link(Node from, Node to)
{
    Link link = (Link)malloc(sizeof(*link));
    link->to = to;
    link->distance = calculate_distance(from,to);
    return link;
}

Node create_node(void* components, int d_count)
{
    Node n = new node(components, d_count);
    return n;
}

float add_neighbour_node(Node from, Node to)
{
    if(from == NULL || to == NULL || from == to)
        return -1;
    
    Link new_link = create_link(from, to);
    auto result = from->neighbours.insert(new_link);

    if (!result.second) {
        free(new_link);
        return -1;
    }
    return new_link->distance;
}


void destroy_node(Node n)
{
    free(n->components);
    for (Link l : n->neighbours) {
        free(l);
    }
    n->neighbours.clear();
    delete n;
}

Graph create_graph(char type, int k, int dimensions)
{
    Graph g = new graph(type, k, dimensions);
    return g;
}

Node add_node_graph(Graph g, int d_count, void* components)
{
    if(d_count != g->dimensions)
        return NULL;
    
    Node n = create_node(components, d_count);
    g->nodes.push_back(n);

    return n;
}

void destroy_graph(Graph g)
{
    for (int i = 0; i < g->nodes.size(); ++i) {
        destroy_node(g->nodes[i]);
    }
    g->nodes.clear();

    delete g;
}


// Calculates and returns the distance between two instances
// or -1 if the dimentions do not match
float calculate_distance(Node a, Node b)
{
    int dim = a->d_count;
    // If the dimentions do not match, skip return error code -1
    if (dim != b->d_count)
        return -1;

    // Calculate the Euclidian Distance
    float sum = 0;

    float* matrix_a = (float*)a->components;
    float* matrix_b = (float*)b->components;
    for(int i=0; i < dim; i++)
    {
        float fact = pow(matrix_a[i] - matrix_b[i], 2);
        sum += fact;    
    }
    return sqrt(sum);

}


// Creates a candidate representation
Candidate create_candidate(Node to, Node query)
{
    Candidate cand = (Candidate)malloc(sizeof(*cand));
    cand->to = to;
    cand->distance = calculate_distance(query,to);
    return cand;
}

//=================== Help Functions ========================//
// The following functions are not included in the interface //
