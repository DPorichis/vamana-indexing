#include "vamana.h"
#include <cmath>

using namespace std;

// Given a Graph with unconnected nodes, it produces a fully connected graph
// Used for creating the starting state ONLY
// Returns 0 when no errors occur or -1 in error state
int init_dummy_graph(Graph g)
{
    for(int i=0; i < g->nodes.size(); i++)
    {
        Node from = g->nodes[i];
        for(int j=0; j < g->nodes.size(); j++)
        {
            if(j!=i)
            {
                Node to = g->nodes[j];
                from->neighbours.push_back(create_link(from, to));
            }
        }
    }
    return 0;
}

// Creates a link from-to
Link create_link(Node from, Node to)
{
    Link link = (Link)malloc(sizeof(*link));
    link->to = to;
    link->distance = calculate_distance(from,to);
    return link;
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
        float fact = pow(matrix_a[i] + matrix_b[i], 2);
        sum += fact;
    }
    return sqrt(sum);

}