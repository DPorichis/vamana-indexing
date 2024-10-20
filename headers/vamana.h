#include "graph.h"
#pragma once

// Graph Initialization
int init_dummy_graph(void);

// Alg 1
set<Candidate>* gready_search(Graph g, Node s, Node query);

// Alg 2
int robust_prunning(void);

// Alg 3
int create_vamana_index(void);

// Benchmarking
int evaluate_index(void);
