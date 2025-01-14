bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

// Creates a graph based on the passed options 
Graph graph_creation(Options opt);
// Retrives a graph based on the passed options 
Graph graph_retrival(Options opt);

// Performs indicated queries calculating accuracy metrics
Stats perform_queries_with_accuracy(Graph graph, Options opt);

// Performs indicated queries without calculating accuracy metrics
Stats perform_queries_without_accuracy(Graph graph, Options opt);

// Parallel implementations of the above. Splits the queries in n threads, executing them concurrently

Stats perform_queries_with_accuracy_parallel(Graph graph, Options opt);
Stats perform_queries_without_accuracy_parallel(Graph graph, Options opt);

// Not used //
void process_single_query(int query_idx, const Graph* graph, const Options* opt, 
                          const std::vector<std::vector<uint32_t>>& groundtruth, Stats* stats);