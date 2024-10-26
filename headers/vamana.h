#include "graph.h"
#pragma once

// Graph Initialization
int init_dummy_graph(Graph g);

// Alg 1
int gready_search(Graph g, Node s, Node query, int L, set<Candidate, CandidateComparator>* neighbours, set<Candidate, CandidateComparator>* visited);

// Alg 2
int robust_prunning(void);

// Alg 3
int create_vamana_index(void);

// Benchmarking
int evaluate_index(void);
