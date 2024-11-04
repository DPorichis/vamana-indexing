#pragma once

#include <vector>
#include <set>
using namespace std;


//===== Structures of Graph Representation =====//

struct node;
typedef struct node* Node;

// Represents a neighbour relationship //
struct link
{
    float distance;
    Node to;

    bool operator==(const link& other) const {
        return to == other.to;
    }

    bool operator<(const link& other) const {
        return to < other.to;
    }

    bool operator>(const link& other) const {
        return to > other.to;
    }
};
typedef struct link* Link;

struct LinkComp
{
    bool operator()(const Link& a, const Link& b) const {
        if (*a < *b) return true;
        if (*b < *a) return false;
        return false;
    }
};

// Represents a node in the Graph //
struct node
{
    int d_count; // object dimension
    void* components;
    int pos;
    set<Link, LinkComp> neighbours;

    // Basic Constractor
    node(void* comp, int dim, int position)
        : components(comp), d_count(dim), pos(position) {}
    node() {}
};


// Represents the entirity of the graph and its meta data //
struct graph
{
    char type; // Type of data in components
    int k; // Defines the k-neighbours bound
    int dimensions; // Defines the dimensions of each element
    vector<Node> nodes;
    
    graph(char t, int kn, int dim)
        : type(t), k(kn), dimensions(dim) {}

};
typedef struct graph* Graph;

// Alias for link, used in when searching for the neighbours//
typedef struct link* Candidate;

struct CandidateComparator {
    bool operator()(const Candidate& a, const Candidate& b) const {
        // For same destination, return false to prevent ordering
        if (a->to == b->to)
            return false;
        // This may cause a bug, float equality not guaranteed
        if (a->distance == b->distance)
            return a->to < b->to;
        
        return a->distance < b->distance; 
    };
};



// ============= Functions ============ //

// Graph Functions //

Graph create_graph(char type, int k, int dimensions);

Node add_node_graph(Graph g, int d_count, void* components, int pos);

void destroy_graph(Graph g);


// Node Functions //

Node create_node(void* components, int d_count);

float add_neighbour_node(Graph g, Node from, Node to);

void destroy_node(Node n);


// Distance and connection related functions //

Link create_link(Graph g, Node from, Node to);

float calculate_distance(Graph g, Node a, Node b);

Candidate create_candidate(Graph g, Node to, Node query);

Candidate create_candidate_copy(Candidate can);

