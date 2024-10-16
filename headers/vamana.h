#include "header.h"
#pragma once

// Graph Initialization
int init_dummy_graph(void);

// Creates a link from-to
Link create_link(Node from, Node to);

// Alg 1
int gready_search(void);

// Alg 2
int robust_prunning(void);

// Alg 3
int create_vamana_index(void);

// Benchmarking
int evaluate_index(void);

// Calculate distance between two instances
float calculate_distance(Node a, Node b);

