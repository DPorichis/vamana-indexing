#include "graph.h"
#include "io.h"
#include <string>
#pragma once


// Given a graph with unconnected n nodes, it creates log(n) random connections
// from each node to anothers in order to produce a connected graph.
// If connections is set to a number different that 0 it will produce this number of random connections
// Returns 0 when no errors occur or -1 in error state
int init_dummy_graph(Graph g, int connections);


// Finds the difference between two candidate sets
void update_dif(set<Candidate, CandidateComparator>* A, set<Candidate, CandidateComparator>* B, set<Candidate, CandidateComparator>* dif);
