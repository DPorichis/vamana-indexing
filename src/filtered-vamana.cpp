#include "filtered-vamana.h"
#include "graph.h"
#include "io.h"

#include <algorithm>
#include <cmath>
#include <set>
#include <map>
#include <iostream>
#include <algorithm>
#include <random>

#include <cstdlib>
#include <ctime>


using namespace std;

void update_dif(set<Candidate, CandidateComparator>* A, set<Candidate, CandidateComparator>* B, set<Candidate, CandidateComparator>* dif);
bool isSubset(set<int> A, set<int> B);



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
int filtered_gready_search(Graph g, Node *S, int s_count, Node query, int k, int L, 
    set<int> query_categories, 
    set<Candidate, CandidateComparator>* neighbours, 
    set<Candidate, CandidateComparator>* visited)
{
    // k must not exceed L
    if(k > L)
        L = k;

    for(int i = 0; i < s_count; i++)
    {
        set<int> intersection;
        set_intersection(query_categories.begin(),
            query_categories.end(),
            S[i]->categories.begin(),
            S[i]->categories.end(),
            inserter(intersection, intersection.begin())
        );
        if(!intersection.empty())
        {
            cout << "candidate inserted" << endl;
            neighbours->insert(create_candidate(g, S[i], query));
        }
    }

    // Calculate the difference between L and V
    set<Candidate, CandidateComparator> difference;
    update_dif(neighbours, visited, &difference);
	int iter = 0;
    while(!difference.empty())
    {
        // // Debugging printing
        cout << "== Iteration "<< iter << " =="<< endl;
        cout << "#dif: " << difference.size() << endl;
        cout << "#vis: " << visited->size() << endl;
        cout << "#nei: " << neighbours->size() << endl;

        // Get the min, which in our set is the first element
        Candidate selected_cand = *difference.begin();

        // Update V to include p*
        Candidate to_insert = create_candidate_copy(selected_cand);
        auto result = visited->insert(to_insert);
        if(!result.second)
            free(to_insert);

        // Update L to include the neighbours of p* that fulfill the category limits
        for (const auto& neig : selected_cand->to->neighbours) 
        {
            set<int> intersection;
            set_intersection(query_categories.begin(),
                query_categories.end(),
                neig->to->categories.begin(),
                neig->to->categories.end(),
                inserter(intersection, intersection.begin())
            );
            if(!intersection.empty())
            {
                Candidate for_insert = create_candidate(g, neig->to, query);
                auto result = neighbours->insert(for_insert);
                // If already in L, free new element so we dont have leaks
                if(!result.second)
                    free(for_insert);
            }
        }

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
int filtered_robust_prunning(Graph g, Node p, set<Candidate, CandidateComparator>* v, float a, int r)
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

            set<int> intersection;
            set_intersection(elem->to->categories.begin(),
                elem->to->categories.end(),
                p->categories.begin(),
                p->categories.end(),
                inserter(intersection, intersection.begin())
            );
            

            if(!isSubset(intersection, selected_cand->to->categories))
            {
                ++it;
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

/*-------- Gready search and prunning need error return values--------------*/
// Vamana index implementation
int create_filtered_vamana_index(Graph* g, const string& filename, int L, int R, float a, int& medoid_pos){
    // // Graph creation and initialization
    // *g = create_graph_from_file(filename, 'f', R);
    // Graph graph = *g;
    // if (graph == NULL) {
    //     cerr << "Error while creating graph from file" << endl;
    //     return -1;
    // }
    // if (init_dummy_graph(graph)) {
    //     cerr << "Error in graph initialization";
    //     return -2;
    // }

    // set<int> categories;
    // map<int, int>* medoids;

    // // Find medoid
    // medoid_pos =  find_filtered_medoid(graph, categories, medoids);
    
    // Node medoid_node = graph->nodes[medoid_pos];

    // // Create random permutation of nodes, vectors is a copy of nodes (not the original)
    // vector<Node> vectors = graph->nodes;
    // random_device rd;
    // mt19937 generator(rd());
    // // Shuffle vector items according to Mersenne Twister engine
    // shuffle(vectors.begin(), vectors.end(), generator);

    // // K for gready search
    // int k = 1;

    // for (int i = 0; i < vectors.size(); i++) {
    //     // Create neighbours and visited sets
    //     // cout << i << endl;
    //     set<Candidate, CandidateComparator>* neighbours = new set<Candidate, CandidateComparator>();
    //     set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();
    //     gready_search(graph, medoid_node, vectors[i], k, L, neighbours, visited);

    //     robust_prunning(graph, vectors[i], visited, a, R);
    //     for (const auto& j : vectors[i]->neighbours) {
    //         // Create temp set
    //         set<Candidate, CandidateComparator>* visited_set = new set<Candidate, CandidateComparator>();
    //         for (const Link& link : j->to->neighbours) {
    //             Candidate to_insert = create_candidate_copy((Link)link);
    //             auto result = visited_set->insert(to_insert);
    //             // If it wasn't inserted, free to manage memory leaks
    //             if (!result.second) {
    //                 free(to_insert);
    //             }

    //         }

    //         Candidate to_insert = create_candidate(graph, vectors[i], j->to);
    //         auto result = visited_set->insert(to_insert);
    //         // If it wasn't inserted, free to manage memory leaks
    //         if (!result.second) {
    //             free(to_insert);
    //         }

    //         if (visited_set->size() > R) {
    //             robust_prunning(graph, j->to, visited_set, a, R);
    //         }
    //         else {
    //             Link for_insert = create_link(graph, j->to, vectors[i]);
    //             auto result = j->to->neighbours.insert(for_insert);
    //             if (!result.second) {
    //                 free(for_insert);
    //             }
    //         }
            
    //         for (const auto& r : *visited_set)
    //             free(r);

    //         delete visited_set;
    //     }
        
    //     for (const auto& r : *neighbours)
    //         free(r);
        
    //     delete neighbours;
        
    //     for (const auto& r : *visited)
    //         free(r);
        
    //     delete visited;

    // }

    return 0;
}



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

bool isSubset(set<int> A, set<int> B)
{
    for(int val: A)
    {
        if(A.find(val) == B.end())
            return false;
    }
    return true;
}


int find_filtered_medoid(Graph graph, set<int> categories, map<int, int>* medoids) {
    
    set<int>::iterator itr;
   
    // Displaying set elements
    for (itr = categories.begin(); itr != categories.end(); itr++)
    {
        int category = *itr;
        int n = graph->nodes.size();
        if (n == 0) {
            cerr << "Empty nodes vector" << endl;
            // return NULL;
            return -1;
        }
        int dimensions = graph->nodes[0]->d_count;
        Node medoid = NULL;
        int medoid_position = -1;
        float min_distance = numeric_limits<float>::max();
        // Calculate distance of each node to all other nodes
        for (int i = 0; i < n; i++) {
            if(graph->nodes[i]->categories.find(category) != 
            graph->nodes[i]->categories.end())
            {
                float total_distance = 0.0f;
                for (int j = 0; j < n; j++) {
                if (i != j) {
                    if(graph->nodes[j]->categories.find(category) != 
                    graph->nodes[j]->categories.end())
                        total_distance += calculate_distance(graph, graph->nodes[i], graph->nodes[j]);
                }
                }
                // Update medoid if we find node with smaller total distance
                if (total_distance < min_distance) {
                    min_distance = total_distance;
                    medoid = graph->nodes[i];
                    medoid_position = i;
                }
            }
        }
        if(medoid_position != -1)
            medoids->insert({category, medoid_position}); 
    }
    return 0;
}