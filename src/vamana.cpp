#include "vamana.h"
#include "graph.h"

#include <algorithm>
#include <cmath>
#include <set>
#include <iostream>

#include <cstdlib>
#include <ctime>


using namespace std;

void update_dif(set<Candidate, CandidateComparator>* A, set<Candidate, CandidateComparator>* B, set<Candidate, CandidateComparator>* dif);

// Given a graph with unconnected n nodes, it creates log(n) random connections
// from each node to anothers in order to produce a connected graph.
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
            // Make sure that we try again if we try to insert a 
            // neighbour that already exists
            int flag = -1;
            while(flag < 0)
            {
                int random_node = std::rand() % g->nodes.size();
                Node to = g->nodes[random_node];
                flag = add_neighbour_node(g, from, to);
            }
        }
    }
    return 0;
}

// Alg 1 from the given paper. Performs gready search on a graph g from starting point s, looking for neighbours of node query
// Returns its results in the neighbours and visited sets pointers that must be passed by the user.
// Returns 0 on correct execution
int gready_search(Graph g, Node s, Node query, int k, int L, 
    set<Candidate, CandidateComparator>* neighbours, 
    set<Candidate, CandidateComparator>* visited)
{
    // k must not exceed L
    if(k > L)
        L = k;

    // Add s to neighbour list
    neighbours->insert(create_candidate(g, s, query));
    Node current = s;

    // Calculate the difference between L and V
    set<Candidate, CandidateComparator> difference;
    update_dif(neighbours, visited, &difference);
	int iter = 0;
    while(!difference.empty())
    {
        // // Debugging printing
        // cout << "== Iteration "<< iter << " =="<< endl;
        // cout << "#dif: " << difference.size() << endl;
        // cout << "#vis: " << visited->size() << endl;
        // cout << "#nei: " << neighbours->size() << endl;

        // Get the min, which in our set is the first element
        Candidate selected_cand = *difference.begin();
        
        // Update L to include the neighbours of p*
        for (const auto& neig : selected_cand->to->neighbours) {
            Candidate for_insert = create_candidate(g, neig->to, query);
            auto result = neighbours->insert(for_insert);
            // If already in L, free new element so we dont have leaks
            if(!result.second)
                free(for_insert);

        }

        // Update V to include p*
        Candidate to_insert = create_candidate_copy(selected_cand);
        auto result = visited->insert(to_insert);
        if(!result.second)
            free(to_insert);

        // Remove items from neighbours until we reach legal size L
        while(neighbours->size() > L)
        {
            // The biggest element in the set is the last one, remove that one
            auto to_erase = std::prev(neighbours->end());
            free(*to_erase);
            neighbours->erase(to_erase);
        }

        // Recalculate the difference
        update_dif(neighbours, visited, &difference);
        iter++;
    }
    return 0;
}

// Alg 2 from the given paper. Performs robust prunning on a node p of graph g based on the v set passed by the user and the arguments
// a, r. The node's p neighbours will be updated accordingly with the prunning.
// Returns 0 on correct execution
int robust_prunning(Graph g, Node p, set<Candidate, CandidateComparator>* v, float a, int r)
{
    // Update V to include neighbours of p
    for (const auto& neig : p->neighbours) {
        Candidate for_insert = create_candidate(g, neig->to, p);
        auto result = v->insert(for_insert);
        if(!result.second)
        {
            free(for_insert);
        }
    }

    // But not p (erase the p if it exists in the v set)
    Candidate erase_self = create_candidate(g, p, p);
    auto it_self = v->find(erase_self);
    if(it_self != v->end())
    {
        const auto elem = *it_self;
        v->erase(it_self);
        free(elem);   
    }
    free(erase_self);

    // Empty all the existing neighbours
    for (auto it = p->neighbours.begin(); it != p->neighbours.end();) {
        const auto elem = *it;
        it = p->neighbours.erase(it);
        free(elem);
    }

    while(!v->empty())
    {

        // Get the min, which in our set is the first element
        Candidate selected_cand = *(v->begin());

        // Insert p* to the neighbours of p
        Candidate for_insert = create_candidate(g, selected_cand->to, p);
        auto result = p->neighbours.insert(for_insert);

        // Stop when we reached R neighbours
        if(p->neighbours.size() == r)
        {
            // cout << "R reached";
            break;
        }
        
        // Distance to compare to
        Candidate target = create_candidate_copy(selected_cand);

        for (auto it = v->begin(); it != v->end();) {
            const auto elem = *it;
            // cout << "Now testing element " << elem << endl;
            // cout << "elem dim: " << elem->to->d_count << endl;
            // cout << "select dim: " << target->to->d_count << endl;
            // fflush(stdout);
            if(elem == NULL) // Debugging
            {
                // cout << "WHY?" << endl;
                // fflush(stdout);
            }
            // if not smaller when multiplied with the a factor
            else if(a * calculate_distance(g ,elem->to, target->to) <= elem->distance)
            {
                // cout << a * calculate_distance(elem->to, target->to) << " < " << elem->distance;
                // fflush(stdout);
                
                // Remove the item from v
                it = v->erase(it); // This will update the iterator
                free(elem);
            } else {
                ++it;  // Next element
            }
        }

        free(target);
    }
    return 0;
}


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

void delete_vis_neigh(set<Candidate, CandidateComparator>* neighbours, set<Candidate, CandidateComparator>* visited)
{
}

// Finds medoid of a graph
// Bad implementation O(d*n^2)
Node find_medoid(Graph g)
{
    Node m = NULL;
    double min_dist = 0;
    for(int i = 0; i < g->nodes.size(); i++)
    {
        //cout << "Node " << i << endl;
        double dist_sum = 0;
        for(int j = 0; j < g->nodes.size(); j++)
            dist_sum += calculate_distance(g, g->nodes[i], g->nodes[j]);
    
        if(dist_sum < min_dist || m == NULL)
        {
            min_dist = dist_sum;
            m = g->nodes[i];
        }
    }
    return m;
}