#include "vamana.h"
#include "vamana-utils.h"
#include "graph.h"
#include "filtered-vamana.h"
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

void * thread_stitched_subgraph(void* arg);


struct category_sync {
    pthread_mutex_t mutex;  // Mutex to protect the index
    std::set<int>::iterator index;              // The index to be synchronized
    set<int>* category_pointer;
    Graph g;

    int L_small; 
    int R_small; 
    int R_stiched; 
    float a; 
    int dimensions;
    int medoid_parallel;

    category_sync(set<int>* cat_p, Graph graph_ptr, int Lsmall, int Rsmall, int Rstiched, float a_param, int dim, int par_med
    ) : category_pointer(cat_p), g(graph_ptr), L_small(Lsmall), R_small(Rsmall), R_stiched(Rstiched), a(a_param), dimensions(dim), medoid_parallel(par_med) {
        pthread_mutex_init(&mutex, nullptr);  // Initialize the mutex
        index = g->all_categories.begin();
    }

    ~category_sync() {
        pthread_mutex_destroy(&mutex);  // Destroy the mutex
    }

    // Method to lock the mutex, modify the index, and unlock the mutex
    bool get_next_category(int* cat) {
        bool not_end = true;
        pthread_mutex_lock(&mutex);  // Lock the mutex
        if(g->all_categories.end() == index)
        {
            not_end = false;
        }
        else
        {
            *cat = *index;
            ++index;
        }
        pthread_mutex_unlock(&mutex);  // Unlock the mutex
        return not_end;
    }
};
typedef struct category_sync* CategorySync;


using namespace std;

// Creates a Stitched vamana index, by creating sub Vamana graphs for each category.
// Returns a map containing these graphs.
Graph create_stiched_vamana_index(const string& filename, int type, int L_small, int R_small, int R_stiched, float a, int dimensions, bool random_init, int parallel_medoid, bool enable_cache) {
    // Graph creation and initialization
    Graph g = create_graph_from_file(filename, type, R_stiched, dimensions, enable_cache);
    
    if(random_init)
    {
        init_dummy_graph(g, R_stiched);
        //cout << "Randinit done" << endl;
    }
    // Perform Vamana initialazation for every sub-graph
    for (auto it = g->all_categories.begin(); it != g->all_categories.end(); ++it) {
        // cout << "Category " << *it << endl;
        Graph subgraph = new graph('f', R_small, dimensions, 0);
        for(int i = 0; i < g->nodes.size(); i++)
        {
            if(g->nodes[i]->categories.find(*it) != g->nodes[i]->categories.end())
                subgraph->nodes.push_back(g->nodes[i]);
        }
        // cout << "Subcategory Found with " << subgraph->nodes.size() << " elements" << endl;
        
        //cout << "Initializing dummy graph of elements : " << subgraph->nodes.size() << endl;
        if (init_dummy_graph(subgraph, 0)) {
            cerr << "Error in graph initialization";
            return NULL;
        }

        // Find medoid
        int category = *it;
        int medoid_pos;
        if(parallel_medoid == 0)
            medoid_pos = find_medoid(subgraph);
        else
            medoid_pos = find_medoid_optimized(subgraph, parallel_medoid);
        
        Node medoid_node = subgraph->nodes[medoid_pos];
        g->medoid_mapping[category] = subgraph->nodes[medoid_pos]->pos;
        // cout << "Medoid Found" << endl;
        
        // Create random permutation of nodes, vectors is a copy of nodes (not the original)
        vector<Node> vectors = subgraph->nodes;
        random_device rd;
        mt19937 generator(rd());
        // Shuffle vector items according to Mersenne Twister engine
        shuffle(vectors.begin(), vectors.end(), generator);

        // K for gready search
        int k = 1;

        for (int i = 0; i < vectors.size(); i++) {
            // Create neighbours and visited sets
            // cout << "Loop " << i << "/" << vectors.size() << endl;
            set<Candidate, CandidateComparator>* neighbours = new set<Candidate, CandidateComparator>();
            set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();
            gready_search(subgraph, medoid_node, vectors[i], k, L_small, neighbours, visited);
            robust_prunning(subgraph, vectors[i], visited, a, R_small);
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

                Candidate to_insert = create_candidate(subgraph, vectors[i], j->to);
                auto result = visited_set->insert(to_insert);
                // If it wasn't inserted, free to manage memory leaks
                if (!result.second) {
                    free(to_insert);
                }

                if (visited_set->size() > L_small) {
                    robust_prunning(subgraph, j->to, visited_set, a, R_small);
                }
                else {
                    Link for_insert = create_link(subgraph, j->to, vectors[i]);
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
        
        subgraph->nodes.clear();
        subgraph->all_categories.clear();
        subgraph->medoid_mapping.clear();

        delete subgraph;
    }
    // Second for-loop of the pseudocode (Maybe useless)
    for(int i = 0; i < g->nodes.size(); i++)
    {
        Node v = g->nodes[i];

        // Create temp set
        set<Candidate, CandidateComparator>* visited_set = new set<Candidate, CandidateComparator>();
        for (const Link& link : v->neighbours) {
            Candidate to_insert = create_candidate_copy((Link)link);
            auto result = visited_set->insert(to_insert);
            // If it wasn't inserted, free to manage memory leaks
            if (!result.second) {
                free(to_insert);
            }
        }

        robust_prunning(g, v, visited_set, a, R_stiched);
    
        for (const auto& r : *visited_set)
            free(r);

        delete visited_set;

    }

    return g;
}

Graph create_stiched_vamana_index_parallel(const string& filename, int type, int L_small, int R_small, int R_stiched, float a, int dimensions, int thread_count, int parallel_medoid, bool enable_cache) {
    // Graph creation and initialization
    Graph g = create_graph_from_file(filename, type, R_stiched, dimensions, enable_cache);

    CategorySync sync = new category_sync(&g->all_categories, g, L_small, R_small, R_stiched, a, dimensions, parallel_medoid);

    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t)*thread_count);
    for(int i = 0; i < thread_count; i++)
    {
        // cout << "thread created" << endl;
        pthread_create((threads + i), NULL, thread_stitched_subgraph, sync);
    }

    for(int i = 0; i < thread_count; i++)
    {
        void* ptr;
        pthread_join(threads[i], &ptr);
    }

    // Second for-loop of the pseudocode (Maybe useless)
    for(int i = 0; i < g->nodes.size(); i++)
    {
        Node v = g->nodes[i];

        // Create temp set
        set<Candidate, CandidateComparator>* visited_set = new set<Candidate, CandidateComparator>();
        for (const Link& link : v->neighbours) {
            Candidate to_insert = create_candidate_copy((Link)link);
            auto result = visited_set->insert(to_insert);
            // If it wasn't inserted, free to manage memory leaks
            if (!result.second) {
                free(to_insert);
            }
        }

        robust_prunning(g, v, visited_set, a, R_stiched);
    
        for (const auto& r : *visited_set)
            free(r);

        delete visited_set;

    }

    delete sync;

    free(threads);

    return g;
}



void * thread_stitched_subgraph(void* arg)
{
    CategorySync sync = (CategorySync)arg;
    while(1)
    {
        int category;
        // cout <<"Selecting a category" << endl;
        if(sync->get_next_category(&category) == false)
        {
             //cout << "Categories run out" << endl;
            return NULL;
        }

        // cout << "Category " << category << endl;
        Graph subgraph = new graph('f', sync->R_small, sync->dimensions, 0);
        Graph g = sync->g;
        for(int i = 0; i < g->nodes.size(); i++)
        {
            if(g->nodes[i]->categories.find(category) != g->nodes[i]->categories.end())
                subgraph->nodes.push_back(g->nodes[i]);
        }
        // cout << "Subcategory Found with " << subgraph->nodes.size() << " elements" << endl;
        
        // cout << "Initializing dummy graph of elements : " << graph->nodes.size() << endl;
        if (init_dummy_graph(subgraph, 0)) {
            cerr << "Error in graph initialization";
            return NULL;
        }

        // Find medoid
        int medoid_pos;
        if (sync->medoid_parallel == 0)
            medoid_pos = find_medoid(subgraph);
        else
            medoid_pos = find_medoid_optimized(subgraph, sync->medoid_parallel);
        Node medoid_node = subgraph->nodes[medoid_pos];
        g->medoid_mapping[category] = subgraph->nodes[medoid_pos]->pos;
        // cout << "Medoid Found" << endl;
        
        // Create random permutation of nodes, vectors is a copy of nodes (not the original)
        vector<Node> vectors = subgraph->nodes;
        random_device rd;
        mt19937 generator(rd());
        // Shuffle vector items according to Mersenne Twister engine
        shuffle(vectors.begin(), vectors.end(), generator);

        // K for gready search
        int k = 1;

        for (int i = 0; i < vectors.size(); i++) {
            // Create neighbours and visited sets
            // cout << "Loop " << i << "/" << vectors.size() << endl;
            set<Candidate, CandidateComparator>* neighbours = new set<Candidate, CandidateComparator>();
            set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();
            gready_search(subgraph, medoid_node, vectors[i], k, sync->L_small, neighbours, visited);
            robust_prunning(subgraph, vectors[i], visited, sync->a, sync->R_small);
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

                Candidate to_insert = create_candidate(subgraph, vectors[i], j->to);
                auto result = visited_set->insert(to_insert);
                // If it wasn't inserted, free to manage memory leaks
                if (!result.second) {
                    free(to_insert);
                }

                if (visited_set->size() > sync->L_small) {
                    robust_prunning(subgraph, j->to, visited_set, sync->a, sync->R_small);
                }
                else {
                    Link for_insert = create_link(subgraph, j->to, vectors[i]);
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
        
        subgraph->nodes.clear();
        subgraph->all_categories.clear();
        subgraph->medoid_mapping.clear();

        delete subgraph;


        }
}
