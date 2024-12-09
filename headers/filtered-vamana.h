#include "graph.h"
#include "io.h"
#include <string>
#include <map>
#include <set>
#pragma once

using namespace std;

// Performs gready search on a graph g from starting the s_count points inside S, looking for neighbours of node query
// Returns its results in the neighbours and visited sets pointers that must be passed by the user.
// Returns 0 on correct execution
int filtered_gready_search(Graph g, Node *S, int s_count, Node query, int k, int L, set<int> query_categories, 
set<Candidate, CandidateComparator>* neighbours, set<Candidate, CandidateComparator>* visited);

// Performs robust prunning on a node p of graph g based on the v set passed by the user and the arguments
// a, r. The node's p neighbours will be updated accordingly with the prunning.
// Returns 0 on correct execution
int filtered_robust_prunning(Graph g, Node p, set<Candidate, CandidateComparator>* v, float a, int r);

// Creates a filtered vamana index as described by the paper provided
int create_filtered_vamana_index(Graph* g, const string& filename, int L, int R, float a, int dimensions);

// Picks medoids for each category node of a graph as described by the papaer
int find_filtered_medoid(Graph graph, set<int> categories, map<int, int>* medoids);

// Creates a Stitched vamana index, by creating sub Vamana graphs for each category.
// Returns a map containing these graphs.
map<int, Graph>* create_stiched_vamana_index(const string& filename, int type, int L_small, int R_small, int R_stiched, float a, int dimensions);
