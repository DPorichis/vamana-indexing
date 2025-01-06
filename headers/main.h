bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

Graph graph_creation(Options opt);
Graph graph_retrival(Options opt);

Stats perform_queries_with_accuracy(Graph graph, Options opt);
Stats perform_queries_without_accuracy(Graph graph, Options opt);

Stats perform_queries_with_accuracy_parallel(Graph graph, Options opt);
Stats perform_queries_without_accuracy_parallel(Graph graph, Options opt);

void process_single_query(int query_idx, const Graph* graph, const Options* opt, 
                          const std::vector<std::vector<uint32_t>>& groundtruth, Stats* stats);