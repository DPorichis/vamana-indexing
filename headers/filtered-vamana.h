#include "graph.h"
#include "io.h"
#include <string>
#include <map>
#include <set>
#pragma once

using namespace std;

// Alg 1 from the given paper. Performs gready search on a graph g from starting point s, looking for neighbours of node query
// Returns its results in the neighbours and visited sets pointers that must be passed by the user.
// Returns 0 on correct execution
int filtered_gready_search(Graph g, Node *S, int s_count, Node query, int k, int L, set<int> query_categories, 
set<Candidate, CandidateComparator>* neighbours, set<Candidate, CandidateComparator>* visited);

// Alg 2 from the given paper. Performs robust prunning on a node p of graph g based on the v set passed by the user and the arguments
// a, r. The node's p neighbours will be updated accordingly with the prunning.
// Returns 0 on correct execution
int filtered_robust_prunning(Graph g, Node p, set<Candidate, CandidateComparator>* v, float a, int r);

// Alg 3
int create_filtered_vamana_index(Graph* g, const string& filename, int L, int R, float a, int dimensions);

// Finds the medoid node of a graph
// int find_medoid(const vector<Node>& nodes);
int find_filtered_medoid(Graph graph, set<int> categories, map<int, int>* medoids);