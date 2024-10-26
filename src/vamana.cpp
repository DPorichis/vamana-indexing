#include "vamana.h"
#include "graph.h"

#include <algorithm>
#include <cmath>
#include <set>
#include <iostream>

#include <cstdlib>
#include <ctime>


using namespace std;

Candidate create_candidate(Node to, Node query);

void update_dif(set<Candidate, CandidateComparator>* A, set<Candidate, CandidateComparator>* B, set<Candidate, CandidateComparator>* dif);

// Given a Graph with unconnected nodes, it produces a fully connected graph
// Used for creating the starting state ONLY
// Returns 0 when no errors occur or -1 in error state
int init_dummy_graph(Graph g)
{
    std::srand(static_cast<unsigned int>(std::time(0)));
    int min_neigh = ceil(log2(g->nodes.size()) + 1);

    for(int i=0; i < g->nodes.size(); i++)
    {
        Node from = g->nodes[i];
        for(int j=0; j < min_neigh; j++)
        {
            int flag = -1;
            while(flag < 0)
            {
                int random_node = std::rand() % g->nodes.size();
                Node to = g->nodes[random_node];
                flag = add_neighbour_node(from, to);
            }
        }
    }
    return 0;
}

int gready_search(Graph g, Node s, Node query, int L, 
    set<Candidate, CandidateComparator>* neighbours, 
    set<Candidate, CandidateComparator>* visited)
{
    if(g->k > L)
        L = g->k;
    neighbours->insert(create_candidate(s, query));
    Node current = s;
    set<Candidate, CandidateComparator> difference;
    update_dif(neighbours, visited, &difference);
	int iter = 0;
    while(!difference.empty())
    {
        cout << "== Iteration "<< iter << " =="<< endl;
        cout << "#dif: " << difference.size() << endl;
        cout << "#vis: " << visited->size() << endl;
        cout << "#nei: " << neighbours->size() << endl;
        Candidate selected_cand = NULL;
        // DEN XREIAZETAI KAN EINAI SET ZHTA TO MIN 
        for (const auto& elem : difference) {
            if(selected_cand == NULL || selected_cand->distance > elem->distance)
            {
                selected_cand = elem;
            }
        }
        for (const auto& neig : selected_cand->to->neighbours) {
            Candidate for_insert = create_candidate(neig->to, query);
            auto result = neighbours->insert(for_insert);
            if(!result.second)
                free(for_insert);

        }
        visited->insert(create_candidate_copy(selected_cand));
        // Remove items from neighbours until we reach legal size L
        while(neighbours->size() > L)
        {
            auto to_erase = std::prev(neighbours->end());
            free(*to_erase);
            neighbours->erase(to_erase);
        }
        update_dif(neighbours, visited, &difference);
        iter++;
    }
    return 0;
}


// Checks if the difference is the empty set
void update_dif(set<Candidate, CandidateComparator>* A, set<Candidate, CandidateComparator>* B, set<Candidate, CandidateComparator>* dif)
{
    dif->clear();
    std::set_difference(A->begin(), A->end(), B->begin(), B->end(),
    std::inserter(*dif, dif->end()),
    [](const Candidate& a, const Candidate& b) {
        return a->to < b->to;
    });
}

void delete_vis_neigh(set<Candidate, CandidateComparator>* neighbours, set<Candidate, CandidateComparator>* visited)
{
}