#include "filtered-vamana.h"
#include "vamana-utils.h"
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

struct category_sync {
    set<int>* category_subset;
    Graph g;
    vector<Node>* shuffled_vec;

    int L;
    int R;
    float a;
    int dimensions;

    category_sync(Graph graph_ptr, int Lsmall, int Rsmall, float a_param, int dim, vector<Node>* vec
    ) : g(graph_ptr), L(Lsmall), R(Rsmall), a(a_param), dimensions(dim), shuffled_vec(vec) {
        category_subset = new set<int>;
    }

    ~category_sync() {
        delete category_subset;
    }

};
typedef struct category_sync* CategorySync;


void * thread_filtered_subgraph(void* arg);


// Performs gready search on a graph g from starting the s_count points inside S, looking for neighbours of node query
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

    // Input all the correct starting points
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
        // cout << "== Iteration "<< iter << " =="<< endl;
        // cout << "#dif: " << difference.size() << endl;
        // cout << "#vis: " << visited->size() << endl;
        // cout << "#nei: " << neighbours->size() << endl;

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

// Performs robust prunning on a node p of graph g based on the v set passed by the user and the arguments
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
            
            // Continue close
            if(!isSubset(intersection, target->to->categories))
            {
                ++it;
            }
            // if not smaller when multiplied with the a factor
            else if(a * g->calculate_distance(g ,elem->to, target->to) <= elem->distance)
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

// Creates a filtered vamana index as described by the paper provided
int create_filtered_vamana_index(Graph* g, const string& filename, int L, int R, float a, int dimensions, bool random_init, bool enable_cache){
    // Graph creation and initialization

    *g = create_graph_from_file(filename, 'f', R, dimensions, enable_cache);
    Graph graph = *g;
    if (graph == NULL) {
        cerr << "Error while creating graph from file" << endl;
        return -1;
    }

    // Connecting the nodes if requested
    if(random_init)
        init_dummy_graph(graph);


    // Find medoids
    find_filtered_medoid(graph, graph->all_categories, &graph->medoid_mapping);
    
    // Create random permutation of nodes, vectors is a copy of nodes (not the original)
    vector<Node> vectors = graph->nodes;
    random_device rd;
    mt19937 generator(rd());
    
    // Shuffle vector items according to Mersenne Twister engine
    shuffle(vectors.begin(), vectors.end(), generator);

    // K for gready search
    int k = 1;

    for (int i = 0; i < vectors.size(); i++) {

        int s_count = vectors[i]->categories.size();
        Node* S = (Node *)malloc(sizeof(*S)*s_count);

        int j = 0;
        for (const int& val : vectors[i]->categories) {
            S[j] = graph->nodes[graph->medoid_mapping[val]];
            j++;
        }

        // Create neighbours and visited sets
        // cout << i << endl;
        set<Candidate, CandidateComparator>* neighbours = new set<Candidate, CandidateComparator>();
        set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();
        
        filtered_gready_search(graph, S, s_count, vectors[i], 0, L, vectors[i]->categories, neighbours, visited);
        
        filtered_robust_prunning(graph, vectors[i], visited, a, R);

        for (const auto& j : vectors[i]->neighbours) {
            Link to_insert = create_link(graph, j->to, vectors[i]);
            auto result = j->to->neighbours.insert(to_insert);
            // If it wasn't inserted, free to manage memory leaks
            if (!result.second) {
                free(to_insert);
            }
            
            if (j->to->neighbours.size() > R) {
                set<Candidate, CandidateComparator>* temp_visited = new set<Candidate, CandidateComparator>();
                for(const auto& neigh : j->to->neighbours)
                {
                    Candidate clone = create_candidate_copy((Candidate)neigh);
                    auto result = temp_visited->insert(clone);
                    // If it wasn't inserted, free to manage memory leaks
                    if (!result.second) {
                        free(clone);
                    }   
                }

                filtered_robust_prunning(graph, j->to, temp_visited, a, R);

                for (const auto& r : *temp_visited)
                    free(r);
                delete temp_visited;
            }
            
        }
        
        for (const auto& r : *neighbours)
            free(r);
        
        delete neighbours;
        
        for (const auto& r : *visited)
            free(r);
        
        delete visited;

        free(S);

    }

    return 0;
}


// Creates a filtered vamana index as described by the paper provided
int create_filtered_vamana_index_parallel(Graph* g, const string& filename, int L, int R, float a, int dimensions, int thread_count){
    
    // Graph creation and initialization
    *g = create_graph_from_file(filename, 'f', R, dimensions, false);
    Graph graph = *g;
    if (graph == NULL) {
        cerr << "Error while creating graph from file" << endl;
        return -1;
    }

    // Find medoids
    find_filtered_medoid(graph, graph->all_categories, &graph->medoid_mapping);
    
    vector<Node> vectors = graph->nodes;
    random_device rd;
    mt19937 generator(rd());

    // Shuffle vector items according to Mersenne Twister engine
    shuffle(vectors.begin(), vectors.end(), generator);

    // K for gready search
    int k = 1;

    CategorySync* thread_args = (CategorySync*)malloc(sizeof(*thread_args)*thread_count);
    for(int i = 0; i < thread_count; i++)
    {
        thread_args[i] = new category_sync(graph, L, R, a, dimensions, &vectors);
    }

    vector<int> vec(graph->all_categories.begin(), graph->all_categories.end());
    std::shuffle(vec.begin(), vec.end(), generator);

    for (size_t i = 0; i < vec.size(); i++)
        thread_args[i%thread_count]->category_subset->insert(vec[i]);

    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t)*thread_count);
    for(int i = 0; i < thread_count; i++)
    {
        // cout << "thread created" << endl;
        pthread_create((threads + i), NULL, thread_filtered_subgraph, (thread_args[i]));
    }

    for(int i = 0; i < thread_count; i++)
    {
        void* ptr;
        pthread_join(threads[i], &ptr);
    }

    for(int i = 0; i < thread_count; i++)
    {
        delete thread_args[i];
    }

    delete thread_args;

    free(threads);


    return 0;
}


void * thread_filtered_subgraph(void* arg)
{
    CategorySync sync = (CategorySync)arg;
    Graph graph = sync->g;
    for (int i = 0; i < sync->shuffled_vec->size(); i++) {
        if(sync->category_subset->find(*(sync->shuffled_vec->at(i)->categories.begin())) == sync->category_subset->end())
            continue;
        int s_count = sync->shuffled_vec->at(i)->categories.size();
        Node* S = (Node *)malloc(sizeof(*S)*s_count);

        int j = 0;
        for (const int& val : sync->shuffled_vec->at(i)->categories) {
            S[j] = graph->nodes[graph->medoid_mapping[val]];
            j++;
        }

        // Create neighbours and visited sets
        // cout << i << endl;
        set<Candidate, CandidateComparator>* neighbours = new set<Candidate, CandidateComparator>();
        set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();
        
        filtered_gready_search(graph, S, s_count, sync->shuffled_vec->at(i), 0, sync->L, sync->shuffled_vec->at(i)->categories, neighbours, visited);
        
        filtered_robust_prunning(graph, sync->shuffled_vec->at(i), visited, sync->a, sync->R);

        for (const auto& j : sync->shuffled_vec->at(i)->neighbours) {
            Link to_insert = create_link(graph, j->to, sync->shuffled_vec->at(i));
            auto result = j->to->neighbours.insert(to_insert);
            // If it wasn't inserted, free to manage memory leaks
            if (!result.second) {
                free(to_insert);
            }
            
            if (j->to->neighbours.size() > sync->R) {
                set<Candidate, CandidateComparator>* temp_visited = new set<Candidate, CandidateComparator>();
                for(const auto& neigh : j->to->neighbours)
                {
                    Candidate clone = create_candidate_copy((Candidate)neigh);
                    auto result = temp_visited->insert(clone);
                    // If it wasn't inserted, free to manage memory leaks
                    if (!result.second) {
                        free(clone);
                    }   
                }

                filtered_robust_prunning(graph, j->to, temp_visited, sync->a, sync->R);

                for (const auto& r : *temp_visited)
                    free(r);
                delete temp_visited;
            }
            
        }
        
        for (const auto& r : *neighbours)
            free(r);
        
        delete neighbours;
        
        for (const auto& r : *visited)
            free(r);
        
        delete visited;

        free(S);

    }

    return NULL;

}


/******** OTHER USEFUL FUNCTIONS **********/

// Finds the difference between sets A and B and returns it in set dif
// The set is sorted based on the distance of each candidate so the min can be retrived just by looking at the first element.

bool isSubset(set<int> A, set<int> B)
{
    for(int val: A)
    {
        if(A.find(val) == B.end())
            return false;
    }
    return true;
}

// Picks medoids for each category node of a graph as described by the papaer
int find_filtered_medoid(Graph graph, set<int> categories, map<int, int>* medoids) {
    
    set<int>::iterator itr;
   
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
        
        // Select a random node of correct type
        vector<int> indexes;
        for (int i = 0; i < n; i++) {
            if(graph->nodes[i]->categories.find(category) != 
            graph->nodes[i]->categories.end())
            {
                indexes.push_back(i);
            }
        }
        medoid_position = indexes[rand() % indexes.size()];
        medoids->insert({category, medoid_position}); 
    }
    // Insert medoids map to graph
    graph->medoid_mapping = *medoids;
    return 0;
}

// === Bruteforce calculation of medoids - [Not used] === //
int find_accurate_filtered_medoid(Graph graph, set<int> categories, map<int, int>* medoids) {
    
    set<int>::iterator itr;
   
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
        }
        if(medoid_position != -1)
            medoids->insert({category, medoid_position}); 
    }
    // Insert medoids map to graph
    graph->medoid_mapping = *medoids;
    return 0;
}