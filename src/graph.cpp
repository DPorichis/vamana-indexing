// This file contains the graph structure implementation and relevant functions
#include "header.h"
#include "graph.h"
#include <iostream>
#include <cmath>

using namespace std;

// Creates a link from-to
Link create_link(Graph g, Node from, Node to)
{
    Link link = (Link)malloc(sizeof(*link));
    link->to = to;
    link->distance = calculate_distance(g, from, to);
    return link;
}

Node create_node(void* components, int d_count)
{
    Node n = new node(components, d_count);
    return n;
}

float add_neighbour_node(Graph g, Node from, Node to)
{
    if(from == NULL || to == NULL || from == to)
        return -1;
    
    Link new_link = create_link(g, from, to);
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
double calculate_distance(Graph g, Node a, Node b)
{
    int dim = a->d_count;
    // If the dimentions do not match, skip return error code -1
    if (dim != b->d_count)
    {    
        cout << "Not matching dimentions " << a->d_count << " != " << b->d_count << endl;
        return -1;
    }
    return g->find_distance(a->components, b->components, dim);
}

double calculate_int(void* a, void* b, int dim)
{
    int* v_a = (int*)a;
    int* v_b = (int*)b;
    double sum = 0.0f;

    for (int i = 0; i < dim; ++i) {
        float diff = (float)(v_a[i] - v_b[i]);
        sum += diff * diff;
    }

    return sqrt(sum);
}

double calculate_char(void* a, void* b, int dim)
{
    unsigned char* v_a = (unsigned char*)a;
    unsigned char* v_b = (unsigned char*)b;
    double sum = 0.0f;
    for (int i = 0; i < dim; ++i) {
        float diff = (float)((int)v_a[i] - (int)v_b[i]);
        sum += diff * diff;
    }

    return sqrt(sum);
}

double calculate_float(void* a, void* b, int dim)
{
    float* v_a = (float*)a;
    float* v_b = (float*)b;
    float sum = 0.0f;

    for (int i = 0; i < dim; ++i) {
        float diff = v_a[i] - v_b[i];
        sum += diff * diff;
    }

    return sqrt(sum);
}

// Creates a candidate representation
Candidate create_candidate(Graph g, Node to, Node query)
{
    Candidate cand = (Candidate)malloc(sizeof(*cand));
    cand->to = to;
    cand->distance = calculate_distance(g, query, to);
    return cand;
}

Candidate create_candidate_copy(Candidate cand)
{
    Candidate cand_copy = (Candidate)malloc(sizeof(*cand));
    cand_copy->to = cand->to;
    cand_copy->distance = cand->distance;
    return cand_copy;    
}

//=================== Help Functions ========================//
// The following functions are not included in the interface //
