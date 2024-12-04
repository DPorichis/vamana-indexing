#pragma once

#include <vector>
#include <set>
#include <map>
using namespace std;


//===== Structures of Graph Representation =====//

struct node;
typedef struct node* Node;

// Represents a neighbour relationship //
struct link
{
    double distance;
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

// Link comperator is based on the to field of each link
// (what pointer it points to)
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
    set<int> categories;

    // Basic Constractor
    node(void* comp, int dim, int position)
        : components(comp), d_count(dim), pos(position) {}
    node() {}
};


// Distance calculation functions for each type of data //

typedef double (*DistanceFunc)(void*, void*, int);

double calculate_int(void* a, void* b, int dim);
double calculate_char(void* a, void* b, int dim);
double calculate_float(void* a, void* b, int dim);

// Represents the entirity of the graph and its meta data //
struct graph
{
    char type; // Type of data in components
    int k; // Defines the k-neighbours bound
    int dimensions; // Defines the dimensions of each element
    DistanceFunc find_distance; // Selected calculate function based on the data type
    vector<Node> nodes; // All the vector Nodes
    int unfiltered_medoid;
    set<int> all_categories;
    map<int,int> medoid_mapping;
    
    // Basic constructor
    graph(char t, int kn, int dim)
        : type(t), k(kn), dimensions(dim), unfiltered_medoid(0)
    {
        // Select the right function depending on the data type
        if(t == 'f')
            find_distance = calculate_float;
        else if(t == 'c')
            find_distance = calculate_char;
        else
            find_distance = calculate_int;
    }

    graph(char t, int kn, int dim, bool filt)
    : type(t), k(kn), dimensions(dim) 
    {
        // Select the right function depending on the data type
        if(t == 'f')
            find_distance = calculate_float;
        else if(t == 'c')
            find_distance = calculate_char;
        else
            find_distance = calculate_int;

        if(filt == true)
            unfiltered_medoid = -1;
        else
            unfiltered_medoid = 0;
    }

};
typedef struct graph* Graph;



// Alias for link, used in when searching for the neighbours //
typedef struct link* Candidate;

// Candidates are stored based on the distance attribute, but they are considered
// equal when pointing to the same thing
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


// Creates a graph and initializes all of the meta data
Graph create_graph(char type, int k, int dimensions);

// Adds a node for a given point to the graph, and returns a pointer to it
// Returns NULL if the dimensions dont match with the graph selected for insertion
Node add_node_graph(Graph g, int d_count, void* components, int pos, set<int> categories);
Node add_node_graph(Graph g, int d_count, void* components, int pos);



// Destroys the graph and deletes all of its data, 
// including the points that were allocated by the user
void destroy_graph(Graph g);


// Node Functions //

// Creates a node representation for the given data
Node create_node(void* components, int d_count, int pos, set<int> categories);
Node create_node(void* components, int d_count, int pos);

// Adds a Node to as a neighbour to node from in the given graph G
// Returns the distance of the two neighbours, or -1 in error state
float add_neighbour_node(Graph g, Node from, Node to);

// Destroys the node representation and all of its data
// including the point that was allocated by the user
void destroy_node(Node n);


// Distance and connection related functions //

// Creates a link representation for the connection of two nodes
// of graph g
Link create_link(Graph g, Node from, Node to);

// Wrapper function for calling the graph function given in the graph's meta data
// returning error code -1 when the dimentions of the two nodes are not the same
double calculate_distance(Graph g, Node a, Node b);

// Creates a candiadate represantation for two nodes
// (Its the same thing as with create_link, used for better understanding)
Candidate create_candidate(Graph g, Node to, Node query);

// Creates and returns an exact copy of a candidate, used for creating duplicates
// of a candidate to prevent accidental freeing.
Candidate create_candidate_copy(Candidate can);

