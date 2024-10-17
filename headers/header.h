#pragma once

#include <vector>
#include <set>

using namespace std;

struct link;
typedef struct link* Link;

struct node
{
    int d_count; // object dimension
    void* components;
    int n_count;
    vector<Link> neighbours;
};
typedef struct node* Node;

struct link
{
    float distance;
    Node to;
};

struct graph
{
    char type; // Type of data in components
    int k; // Defines the k-neighbours bound
    int dimensions; // Defines the dimensions of each element
    vector<Node> nodes;
};
typedef struct graph* Graph;

struct candidate
{
    float distance;
    Node to;
};
typedef struct candidate* Candidate;