#include "vamana.h"
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
#include <pthread.h>

#include <cstdlib>
#include <ctime>


using namespace std;

void* thread_medoid(void * arg);

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

/*-------- Gready search and prunning need error return values--------------*/
// Vamana index implementation
int create_vamana_index(Graph* g, const string& filename, int L, int R, float a,int& medoid_pos, int dimensions, char random_medoid, int parallel, bool enable_cache) {
    // Graph creation and initialization
    *g = create_graph_from_file(filename, 'f', R, dimensions, enable_cache);            
    Graph graph = *g;
    if (graph == NULL) {
        cerr << "Error while creating graph from file" << endl;
        return -1;
    }
    if (init_dummy_graph(graph)) {
        cerr << "Error in graph initialization";
        return -2;
    }

    // Find medoid
    if(random_medoid == 'n')
    {
        if(parallel > 0)
            medoid_pos = find_medoid_optimized(graph, parallel);
        else
            medoid_pos =  find_medoid(graph);
    }
    else if(random_medoid == 's')
        medoid_pos =  find_random_medoid(graph);
    else
    {
        srand(time(0));
        medoid_pos = rand() % graph->nodes.size();

    }

    graph->unfiltered_medoid = medoid_pos;
    
    Node medoid_node = graph->nodes[medoid_pos];

    // Create random permutation of nodes, vectors is a copy of nodes (not the original)
    vector<Node> vectors = graph->nodes;
    random_device rd;
    mt19937 generator(rd());
    // Shuffle vector items according to Mersenne Twister engine
    shuffle(vectors.begin(), vectors.end(), generator);

    // K for gready search
    int k = 1;

    for (int i = 0; i < vectors.size(); i++) {
        // Create neighbours and visited sets
        // cout << i << endl;
        set<Candidate, CandidateComparator>* neighbours = new set<Candidate, CandidateComparator>();
        set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();
        gready_search(graph, medoid_node, vectors[i], k, L, neighbours, visited);

        robust_prunning(graph, vectors[i], visited, a, R);
        for (const auto& j : vectors[i]->neighbours) {
            // Create temp set
            set<Candidate, CandidateComparator>* visited_set = new set<Candidate, CandidateComparator>();
            for (const Link& link : j->to->neighbours) {
                Candidate to_insert = create_candidate_copy((Link)link);
                auto result = visited_set->insert(to_insert);
                // If it wasn't inserted, free to manage memory leaks
                if (!result.second) {
                    free(to_insert);
                }

            }

            Candidate to_insert = create_candidate(graph, vectors[i], j->to);
            auto result = visited_set->insert(to_insert);
            // If it wasn't inserted, free to manage memory leaks
            if (!result.second) {
                free(to_insert);
            }

            if (visited_set->size() > R) {
                robust_prunning(graph, j->to, visited_set, a, R);
            }
            else {
                Link for_insert = create_link(graph, j->to, vectors[i]);
                auto result = j->to->neighbours.insert(for_insert);
                if (!result.second) {
                    free(for_insert);
                }
            }
            
            for (const auto& r : *visited_set)
                free(r);

            delete visited_set;
        }
        
        for (const auto& r : *neighbours)
            free(r);
        
        delete neighbours;
        
        for (const auto& r : *visited)
            free(r);
        
        delete visited;

    }

    return 0;
}


int find_medoid(Graph graph) {
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
        float total_distance = 0.0f;
        for (int j = 0; j < n; j++) {
            if (i != j) {
                total_distance += calculate_distance_without_cache(graph, graph->nodes[i], graph->nodes[j]);
            }
        }
        // Update medoid if we find node with smaller total distance
        if (total_distance < min_distance) {
            min_distance = total_distance;
            medoid = graph->nodes[i];
            medoid_position = i;
        }
    }
    return medoid_position;
}


int find_medoid_optimized(Graph graph, int thread_count) {
    int n = graph->nodes.size();
    if (n == 0) {
        cerr << "Empty nodes vector" << endl;
        // return NULL;
        return -1;
    }

    Subproblem subs = (Subproblem)malloc(sizeof(*subs)*thread_count);
    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t)*thread_count);
    for(int i = 0; i < thread_count; i++)
    {
        // Pass the number of iterations inside the thread's spot
        subs[i].start = i * (n / thread_count);
        subs[i].end = (i + 1)*(n / thread_count);
        subs[i].g = graph;
        pthread_create((threads + i), NULL, thread_medoid, (subs + i));
    }

    int dimensions = graph->nodes[0]->d_count;
    Node medoid = NULL;
    int medoid_position = -1;
    float min_distance = numeric_limits<float>::max();


    for(int i = 0; i < thread_count; i++)
    {
        // One thrad at a time
        void* ptr;
        pthread_join(threads[i], &ptr);
        if (subs[i].distance < min_distance) {
            min_distance = subs[i].distance;
            medoid = graph->nodes[subs[i].result];
            medoid_position = subs[i].result;
        }
    }

    free(subs);
    free(threads);

    return medoid_position;
}

int find_random_medoid(Graph graph) {
    int n = graph->nodes.size();
    if (n == 0) {
        cerr << "Empty nodes vector" << endl;
        // return NULL;
        return -1;
    }

    srand(time(0));
    set<int> indexes;

    for (int i = 0; i < n*0.1; ++i) {
        int random_item = rand() % n;
        indexes.insert(random_item);
    }

    int dimensions = graph->nodes[0]->d_count;
    Node medoid = NULL;
    int medoid_position = -1;
    float min_distance = numeric_limits<float>::max();
    // Calculate distance of each node to all other nodes
    for (int i : indexes) {
        float total_distance = 0.0f;
        for (int j : indexes) {
            if (i != j) {
                total_distance += calculate_distance_without_cache(graph, graph->nodes[i], graph->nodes[j]);
            }
        }
        // Update medoid if we find node with smaller total distance
        if (total_distance < min_distance) {
            min_distance = total_distance;
            medoid = graph->nodes[i];
            medoid_position = i;
        }
    }
    return medoid_position;
}



void* thread_medoid(void * arg)
{
    Subproblem sub = (Subproblem)arg;

    int dimensions = sub->g->nodes[0]->d_count;
    Node medoid = NULL;
    int medoid_position = -1;
    float min_distance = numeric_limits<float>::max();
    // Calculate distance of each node to all other nodes
    for (int i = sub->start; i < sub->end; i++) {
        float total_distance = 0.0f;
        for (int j = 0; j < sub->g->nodes.size(); j++) {
            if (i != j) {
                total_distance += calculate_distance_without_cache(sub->g, sub->g->nodes[i], sub->g->nodes[j]);
            }
        }
        // Update medoid if we find node with smaller total distance
        if (total_distance < min_distance) {
            min_distance = total_distance;
            medoid = sub->g->nodes[i];
            medoid_position = i;
        }
    }

    sub->result = medoid_position;
    sub->distance = min_distance;

    return NULL;
}