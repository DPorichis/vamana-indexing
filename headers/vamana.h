#include "graph.h"
#include "io.h"
#include <string>
#pragma once

using namespace std;

// Given a graph with unconnected n nodes, it creates log(n) random connections
// from each node to anothers in order to produce a connected graph.
// Returns 0 when no errors occur or -1 in error state
int init_dummy_graph(Graph g);

// Alg 1 from the given paper. Performs gready search on a graph g from starting point s, looking for neighbours of node query
// Returns its results in the neighbours and visited sets pointers that must be passed by the user.
// Returns 0 on correct execution
int gready_search(Graph g, Node s, Node query, int k, int L, set<Candidate, CandidateComparator>* neighbours, set<Candidate, CandidateComparator>* visited);

// Alg 2 from the given paper. Performs robust prunning on a node p of graph g based on the v set passed by the user and the arguments
// a, r. The node's p neighbours will be updated accordingly with the prunning.
// Returns 0 on correct execution
int robust_prunning(Graph g, Node p, set<Candidate, CandidateComparator>* v, float a, int r);

// Alg 3
int create_vamana_index(Graph* g, const string& filename, int L, int R, float a, int& medoid_pos, int dimensions);

// Finds the medoid node of a graph
// int find_medoid(const vector<Node>& nodes);
int find_medoid(Graph graph);
