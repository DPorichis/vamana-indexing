// This file contains the graph structure implementation and relevant functions

#include "header.h"
#include "graph.h"
#include "dist-cache.h"
#include <iostream>
#include <cmath>

using namespace std;

//**** Graph Functions ****//

// Creates a graph and initializes all of the meta data
Graph create_graph(char type, int k, int dimensions, bool enable_cache)
{
    // Call the constructor
    Graph g;
    if (enable_cache)
        g = new graph(type, k, dimensions, 1000);
    else
        g = new graph(type, k, dimensions, 0);
    return g;
}

// Adds a node for a given point to the graph, and returns a pointer to it
// Returns NULL if the dimensions dont match with the graph selected for insertion
Node add_node_graph(Graph g, int d_count, void* components, int pos, set<int> categories)
{
    if(d_count != g->dimensions)
        return NULL;
    
    // Create and add
    Node n = create_node(components, d_count, pos, categories);

    g->nodes.push_back(n);
    g->all_categories.insert(categories.begin(), categories.end());

    return n;
}

// Adds a node for a given point to the graph, and returns a pointer to it
// Returns NULL if the dimensions dont match with the graph selected for insertion
Node add_node_graph(Graph g, int d_count, void* components, int pos)
{
    if(d_count != g->dimensions)
        return NULL;
    
    // Create and add
    Node n = create_node(components, d_count, pos);
    g->nodes.push_back(n);

    return n;
}

// Destroys the graph and deletes all of its data, 
// including the points that were allocated by the user
void destroy_graph(Graph g)
{
    // Delete each node
    for (int i = 0; i < g->nodes.size(); ++i) {
        destroy_node(g->nodes[i]);
    }

    g->nodes.clear();
    g->all_categories.clear();
    g->medoid_mapping.clear();

    if(g->graph_cache != NULL)
        delete g->graph_cache;

    // And yourself
    delete g;
}


//**** Node Functions ****//

// Creates a node representation for the given data
Node create_node(void* components, int d_count, int pos, set<int> categories)
{
    // Call the constructor
    Node n = new node(components, d_count, pos);
    set<int>::iterator itr;
    for (itr = categories.begin(); itr != categories.end(); itr++) 
        n->categories.insert(*itr);
    return n;
}

// Creates a node representation for the given data
Node create_node(void* components, int d_count, int pos)
{
    // Call the constructor
    Node n = new node(components, d_count, pos);
    return n;
}

// Adds a Node to as a neighbour to node from in the given graph G,
// Returns the distance of the two neighbours, or -1 in error state
float add_neighbour_node(Graph g, Node from, Node to)
{
    if(from == NULL || to == NULL || from == to)
        return -1;
    
    // Create and insert
    Link new_link = create_link(g, from, to);
    auto result = from->neighbours.insert(new_link);
    
    // If we failed to insert return error -1
    if (!result.second) {
        free(new_link);
        return -1;
    }

    // Return the distance of the neighbours
    return new_link->distance;
}

// Destroys the node representation and all of its data
// including the point that was allocated by the user
void destroy_node(Node n)
{
    // Free the point that user passed as argument
    free(n->components);

    // Destroy all neighbours
    for (Link l : n->neighbours) {
        free(l);
    }
    n->neighbours.clear();
    n->categories.clear();

    // Destroy self
    delete n;
}

//**** Connection Functions ****//

// Creates a link representation for the connection of two nodes
// of graph g
Link create_link(Graph g, Node from, Node to)
{
    Link link = (Link)malloc(sizeof(*link));
    link->to = to;
    link->distance = g->calculate_distance(g, from, to);
    return link;
}

// Creates a candiadate represantation for two nodes
// (Its the same thing as with create_link, used for better understanding)
Candidate create_candidate(Graph g, Node to, Node query)
{
    Candidate cand = (Candidate)malloc(sizeof(*cand));
    cand->to = to;
    cand->distance = g->calculate_distance(g, query, to);
    return cand;
}

// Creates and returns an exact copy of a candidate, used for creating duplicates
// of a candidate to prevent accidental freeing.
Candidate create_candidate_copy(Candidate cand)
{
    Candidate cand_copy = (Candidate)malloc(sizeof(*cand));
    cand_copy->to = cand->to;
    cand_copy->distance = cand->distance;
    return cand_copy;    
}


//**** Distance Functions ****//

// Wrapper function for calling the graph function given in the graph's meta data
// returning error code -1 when the dimentions of the two nodes are not the same
double calculate_distance_with_cache(Graph g, Node a, Node b)
{
    int dim = a->d_count;
    // If the dimentions do not match, skip return error code -1
    if (dim != b->d_count)
    {    
        cout << "Not matching dimentions " << a->d_count << " != " << b->d_count << endl;
        return -1;
    }

    // Search for the distance there
    double distance = g->graph_cache->getDistance(a, b);
    if(distance >= 0)
    {
        //cout << "Distance found" << endl;
        return distance;
    }
    // If it is not available, calculate it and store it yourself
    distance = g->find_distance(a->components, b->components, dim);
    g->graph_cache->putDistance(a, b, distance);
    
    return distance;
}

double calculate_distance_without_cache(Graph g, Node a, Node b)
{
    int dim = a->d_count;
    // If the dimentions do not match, skip return error code -1
    if (dim != b->d_count)
    {    
        cout << "Not matching dimentions " << a->d_count << " != " << b->d_count << endl;
        return -1;
    }

    // Call the find_distance stored in the graph meta data with the two points as arguments
    return g->find_distance(a->components, b->components, dim);
}

// Distance calculation for int vectors
double calculate_int(void* a, void* b, int dim)
{
    // Cast as int tables
    int* v_a = (int*)a;
    int* v_b = (int*)b;

    double sum = 0.0f; // sum is always a double
    for (int i = 0; i < dim; ++i) {
        float diff = (float)(v_a[i] - v_b[i]);
        sum += diff * diff;
    }

    // return sqrt(sum);
    return sum;
}

// Distance calculation for char vectors
double calculate_char(void* a, void* b, int dim)
{
    // Cast as unsigned char tables
    unsigned char* v_a = (unsigned char*)a;
    unsigned char* v_b = (unsigned char*)b;

    double sum = 0.0f; // sum is always a double
    for (int i = 0; i < dim; ++i) {
        float diff = (float)((int)v_a[i] - (int)v_b[i]);
        sum += diff * diff;
    }

    // return sqrt(sum);
    return sum;
}

// Distance calculation for float vectors
double calculate_float(void* a, void* b, int dim)
{
    // Cast as float tables
    float* v_a = (float*)a;
    float* v_b = (float*)b;
    float sum = 0.0f; // sum is always a double

    for (int i = 0; i < dim; ++i) {
        float diff = v_a[i] - v_b[i];
        sum += diff * diff;
    }

    // return sqrt(sum);
    return sum;
}
