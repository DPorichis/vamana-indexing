#include "graph.h"
#include "io.h"
#include <string>
#pragma once

using namespace std;

// Graph Initialization
int init_dummy_graph(Graph g);

// Alg 1
int gready_search(Graph g, Node s, Node query, int k, int L, set<Candidate, CandidateComparator>* neighbours, set<Candidate, CandidateComparator>* visited);

// Alg 2
int robust_prunning(Graph g, Node p, set<Candidate, CandidateComparator>* v, float a, int r);

// Alg 3
int create_vamana_index(Graph* g, const string& filename, int L, int R);

// Benchmarking
int evaluate_index(void);

// Finds the medoid node of a graph
// int find_medoid(const vector<Node>& nodes);
int find_medoid(Graph graph);
