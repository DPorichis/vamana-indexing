#include "vamana.h"
#include "graph.h"
#include <algorithm>
#include <cmath>
#include <set>

using namespace std;

struct CandidateComparator {
    bool operator()(const Candidate& a, const Candidate& b) const {
        if (a->distance != b->distance) {
            return a->distance < b->distance;
        }
        return a->to < b->to;
    }
};

Candidate create_candidate(Node to, Node query);
void update_dif(set<Candidate, CandidateComparator>* A, set<Candidate, CandidateComparator>* B, set<Candidate, CandidateComparator>* dif);

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
                from->neighbours.insert(create_link(from, to));
            }
        }
    }
    return 0;
}

set<Candidate>* gready_search(Graph g, Node s, Node query)
{
    set<Candidate, CandidateComparator>* neighbours = (set<Candidate, CandidateComparator>*)malloc(sizeof(*neighbours));
    set<Candidate, CandidateComparator> visited;
    neighbours->insert(create_candidate(s, query));
    Node current = s;
    set<Candidate, CandidateComparator> difference;
    update_dif(neighbours, &visited, &difference);
    while(!difference.empty())
    {
        Candidate selected_cand = NULL;
        /// DEN XREIAZETAI KAN EINAI SET ZHTA TO MIN
        for (const auto& elem : difference) {
            if(selected_cand == NULL || selected_cand->distance > elem->distance)
            {
                selected_cand = elem;
            }
        }
        // for(int i = 0; i < selected_cand->to->n_count; i++)
        // {
        //     neighbours->insert(create_candidate(selected_cand->to->neighbours[i]->to, query));
        // }
        visited.insert(selected_cand);
        // Remove items from neighbours until we reach legal size
        while(neighbours->size() > g->k)
        {
            auto to_erase = neighbours->end();
            neighbours->erase(--to_erase);
        }
    }

    set<Candidate>* results = (set<Candidate>*)malloc(sizeof(*results));
    auto to_insert = neighbours->begin();
    auto it = neighbours->begin();
    for (int i = 0; i < g->k && it != neighbours->end(); ++i, ++it)
    {
        results->insert(*it);
    }
    return results;
}


// Checks if the difference is the empty set
void update_dif(set<Candidate, CandidateComparator>* A, set<Candidate, CandidateComparator>* B, set<Candidate, CandidateComparator>* dif)
{
    std::set_difference(A->begin(), A->end(), B->begin(), B->end(),
    std::inserter(*dif, dif->end()));
}

