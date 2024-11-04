#include "vamana.h"
#include "graph.h"
#include "io.h"

#include <algorithm>
#include <cmath>
#include <set>
#include <iostream>
#include <algorithm>
#include <random>

#include <cstdlib>
#include <ctime>


using namespace std;

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
                flag = add_neighbour_node(g, from, to);
            }
        }
    }
    return 0;
}

int gready_search(Graph g, Node s, Node query, int k, int L, 
    set<Candidate, CandidateComparator>* neighbours, 
    set<Candidate, CandidateComparator>* visited)
{
    if(k > L)
        L = k;
    neighbours->insert(create_candidate(g, s, query));
    Node current = s;
    set<Candidate, CandidateComparator> difference;
    update_dif(neighbours, visited, &difference);
	int iter = 0;
    while(!difference.empty())
    {
        // cout << "== Iteration "<< iter << " =="<< endl;
        // cout << "#dif: " << difference.size() << endl;
        // cout << "#vis: " << visited->size() << endl;
        // cout << "#nei: " << neighbours->size() << endl;
        Candidate selected_cand = NULL;
        // DEN XREIAZETAI KAN EINAI SET ZHTA TO MIN 
        for (const auto& elem : difference) {
            if(selected_cand == NULL || selected_cand->distance > elem->distance)
            {
                selected_cand = elem;
            }
        }
        for (const auto& neig : selected_cand->to->neighbours) {
            Candidate for_insert = create_candidate(g, neig->to, query);
            auto result = neighbours->insert(for_insert);
            if(!result.second)
                free(for_insert);

        }

        Candidate to_insert = create_candidate_copy(selected_cand);
        auto result = visited->insert(to_insert);
        if(!result.second)
            free(to_insert);

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


// Robust prunning implementation
int robust_prunning(Graph g, Node p, set<Candidate, CandidateComparator>* v, float a, int r)
{
    for (const auto& neig : p->neighbours) {
        Candidate for_insert = create_candidate(g, neig->to, p);
        auto result = v->insert(for_insert);
        if(!result.second)
        {
            free(for_insert);
        }
    }

    // Erase the p if it exists in the v set
    Candidate erase_self = create_candidate(g, p, p);
    auto it_self = v->find(erase_self);
    if(it_self != v->end())
    {
        const auto elem = *it_self;
        v->erase(it_self);
        free(elem);   
    }
    free(erase_self);

    // Fix this so it doesn't memory leak <3
    // Empty all the existing neighbours
    for (auto it = p->neighbours.begin(); it != p->neighbours.end();) {
        const auto elem = *it;
        it = p->neighbours.erase(it);
        free(elem);
    }

    while(!v->empty())
    {
        Candidate selected_cand = NULL;
        // DEN XREIAZETAI KAN EINAI SET ZHTA TO MIN 
        for (const auto& elem : *v) {
            if(selected_cand == NULL || selected_cand->distance > elem->distance)
            {
                selected_cand = elem;
            }
        }

        Candidate for_insert = create_candidate(g, selected_cand->to, p);
        auto result = p->neighbours.insert(for_insert);

        if(p->neighbours.size() == r)
        {
            // cout << "R reached";
            break;
        }

        Candidate target = create_candidate_copy(selected_cand);

        for (auto it = v->begin(); it != v->end();) {
            const auto elem = *it;
            // cout << "Now testing element " << elem << endl;
            // cout << "elem dim: " << elem->to->d_count << endl;
            // cout << "select dim: " << target->to->d_count << endl;
            // fflush(stdout);
            if(elem == NULL)
            {
                // cout << "WHY?" << endl;
                // fflush(stdout);
            }
            else if(a * calculate_distance(g ,elem->to, target->to) <= elem->distance) // distance bugs because of float may occur
            {
                // cout << a * calculate_distance(elem->to, target->to) << " < " << elem->distance;
                fflush(stdout);
                it = v->erase(it);
                free(elem);
                fflush(stdout);
            } else {
                ++it;  // Only increment if no erase occurs
            }
        }

        free(target);
    }
    return 0;
}

/*-------- Gready search and prunning need error return values--------------*/
// Vamana index implementation
int create_vamana_index(Graph* g, const string& filename, int L, int R) {
    // Graph creation and initialization
    *g = create_graph_from_file(filename, 'f', 10);
    Graph graph = *g;
    if (graph == NULL) {
        cerr << "Error while creating graph from file" << endl;
        return -1;
    }
    if (init_dummy_graph(graph)) {
        cerr << "Error in graph initialization";
        return -2;
    }

    int pos;
    pos = 8736;
    /* UNCOMMENT if is the final execusion. Otherwise pos is 8736, after 1 minute of function execution
    // int pos =  find_medoid(graph->nodes);
    */
    Node medoid_node = graph->nodes[pos];

    // Create random permutation of nodes, vectors is a copy of nodes (not the original)
    vector<Node> vectors = graph->nodes;
    random_device rd;
    mt19937 generator(rd());
    // Shuffle vector items according to Mersenne Twister engine
    shuffle(vectors.begin(), vectors.end(), generator);

    // K for gready search
    int k = 1;

    // a parameter for pruning
    int a = 1.6;

    for (int i = 0; i < vectors.size(); i++) {
        // Create neighbours and visited sets
        // cout << i << endl;
        set<Candidate, CandidateComparator>* neighbours = new set<Candidate, CandidateComparator>();
        set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();
        gready_search(graph, medoid_node, vectors[i], k, L, neighbours, visited);

        robust_prunning(graph, vectors[i], visited, a, R);
        for (const auto& j : vectors[i]->neighbours) {
            // Create temp set
            // Link comp or Cand Comp
            // set<Candidate, LinkComp>* visited_set = &(j->to->neighbours.begin(), j->to->neighbours.end());
            set<Candidate, CandidateComparator>* visited_set = new set<Candidate, CandidateComparator>();
            for (const Link& link : j->to->neighbours) {
                visited_set->insert(create_candidate_copy((Link)link));
            }

            visited_set->insert(create_candidate(graph, vectors[i], j->to));

            if (visited_set->size() > R) {
                robust_prunning(graph, j->to, visited_set, a, R);
            }
            else {
                j->to->neighbours.insert(create_link(graph, j->to, vectors[i]));
            }
            
            delete visited_set;
            // for (const auto& elem_nb : elem->to->neighbours) {
            //     Candidate cand = create_candidate(graph, elem_nb->to, elem->to);
            // }
            // Link for_insert = create_link(graph, )
            // elem->to->neighbours.insert(vectors[i]);
            // add_neighbour_node(graph, elem->to, vectors[i]);
            // if (elem->to->neighbours.size() > R) {
            //     robust_prunning(graph, elem, elem->to->neighbours, a, R);
            //     elem->to->neighbours.erase(vectors[i]);
            // }

        }
    }

    return 0;
}



/******** OTHER USEFUL FUNCTIONS **********/

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

// Calculates euclidean distance of vectors a and b
float euclidean_distance(const float* a, const float* b, int dimensions) {
    float sum = 0.0f;
    for (int i = 0; i < dimensions; ++i) {
        float diff = a[i] - b[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}
// int find_medoid(const vector<Node>& nodes) {
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
                total_distance += euclidean_distance((float*)graph->nodes[i]->components, (float*)graph->nodes[j]->components, dimensions);
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