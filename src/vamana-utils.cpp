#include "vamana-utils.h"
#include "graph.h"
#include "io.h"

#include <algorithm>
#include <cmath>
#include <set>
#include <iostream>
#include <algorithm>
#include <random>
#include <vector>

#include <cstdlib>
#include <ctime>

/******** OTHER USEFUL FUNCTIONS **********/

// Finds the difference between sets A and B and returns it in set dif
// The set is sorted based on the distance of each candidate so the min can be retrived just by looking at the first element.
void update_dif(set<Candidate, CandidateComparator>* A, set<Candidate, CandidateComparator>* B, set<Candidate, CandidateComparator>* dif)
{
    dif->clear();
    std::set_difference(A->begin(), A->end(), B->begin(), B->end(),
    std::inserter(*dif, dif->end()),
    [](const Candidate& a, const Candidate& b) {
        return a->to < b->to;
    });
}


// Given a graph with unconnected n nodes, it creates log(n) random connections
// from each node to anothers in order to produce a connected graph.
// Returns 0 when no errors occur or -1 in error state
int init_dummy_graph(Graph g, int connections)
{
    if(connections == 0)
    {
        int min_neigh = ceil(log2(g->nodes.size()) + 1);
        if(min_neigh == g->nodes.size())
            min_neigh--;
        std::srand(static_cast<unsigned int>(std::time(0)));
        for(int i=0; i < g->nodes.size(); i++)
        {
            Node from = g->nodes[i];
            for(int j=0; j < min_neigh; j++)
            {
                // Make sure that we try again if we try to insert a 
                // neighbour that already exists
                int flag = -1;
                int tried = 0;
                while(flag < 0)
                {
                    int random_node = std::rand() % g->nodes.size();
                    Node to = g->nodes[random_node];
                    flag = add_neighbour_node(g, from, to);
                    tried++;
                    if(tried > 10 && from->neighbours.size() >= min_neigh)
                        return 0;
                }
            }
        }
    }else
    {
        if(connections >= g->nodes.size())
            connections = g->nodes.size() - 1;
        std::srand(static_cast<unsigned int>(std::time(0)));
        for(int i=0; i < connections; i++)
        {
            // Make sure that we try again if we try to insert a 
            // neighbour that already exists
            int flag = -1;
            while(flag < 0)
            {
                int random_from_node = std::rand() % g->nodes.size();
                int random_node = std::rand() % g->nodes.size();
                Node from = g->nodes[random_from_node];
                Node to = g->nodes[random_node];
                flag = add_neighbour_node(g, from, to);
            }
        }
    }
    return 0;
}