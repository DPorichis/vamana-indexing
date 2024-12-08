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

#include <cstdlib>
#include <ctime>


using namespace std;

/*-------------- Gready search and prunning need error return values --------------*/
// Vamana index implementation
map<int, Graph>* create_stiched_vamana_index(const string& filename, int type, int L_small, int R_small, int R_stiched, float a, int dimensions) {
    // Graph creation and initialization
    map<int, Graph>* stiched_mapping = create_stiched_graph_from_file(filename, type, R_small, dimensions);            
    int id = -1;
    for (auto it = stiched_mapping->begin(); it != stiched_mapping->end(); ++it) {
        id++;
        cout << "Graph " << id << "/" << stiched_mapping->size() << endl;
        
        Graph graph = it->second;
        if (graph == NULL) {
            cerr << "Error while creating graph from file" << endl;
            return NULL;
        }

        // cout << "Initializing dummy graph of elements : " << graph->nodes.size() << endl;
        if (init_dummy_graph(graph)) {
            cerr << "Error in graph initialization";
            return NULL;
        }

        // Find medoid
        graph->unfiltered_medoid = find_medoid(graph);
        Node medoid_node = graph->nodes[graph->unfiltered_medoid];
        // cout << "Medoid Found" << endl;
        
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
            // cout << "Loop " << i << "/" << vectors.size() << endl;
            set<Candidate, CandidateComparator>* neighbours = new set<Candidate, CandidateComparator>();
            set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();
            gready_search(graph, medoid_node, vectors[i], k, L_small, neighbours, visited);
            robust_prunning(graph, vectors[i], visited, a, R_small);
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

                if (visited_set->size() > L_small) {
                    robust_prunning(graph, j->to, visited_set, a, R_small);
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
    }
    for (auto it = stiched_mapping->begin(); it != stiched_mapping->end(); ++it) {
        Graph graph = it->second;
        for(int i = 0; i < graph->nodes.size(); i++)
        {
            Node v = graph->nodes[i];

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

            robust_prunning(graph, v, visited_set, a, R_stiched);
        
            for (const auto& r : *visited_set)
                free(r);

            delete visited_set;

        }
    }

    return stiched_mapping;
}

