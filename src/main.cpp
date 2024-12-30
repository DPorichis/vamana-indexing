#include <iostream>
#include <algorithm>
#include <set>
#include "vamana.h"
#include "filtered-vamana.h"
#include "vamana-utils.h"
#include <fstream>
#include <chrono>


using namespace std;

bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

Graph graph_creation(Options opt);
Graph graph_retrival(Options opt);

Stats perform_queries_with_accuracy(Graph graph, Options opt);
Stats perform_queries_without_accuracy(Graph graph, Options opt);

int main(int argc, char* argv[]) {

    Options opt = new options();
    int error = 0;

    string str = argv[1];
    if(str.compare("-config") == 0 && argc == 3)
    {
        error = read_config_file(argv[2], opt);
    }
    else
    {
        error = read_command_line_args(argc, argv, opt);
    }

    if(error != 0)
    {
        cout << "[Argument error detected, exiting...]" << endl;
        delete opt;
        return -1;
    }

    if(opt->printing == 'f')
        print_options(opt);
    
    Graph graph;
    
    auto start = std::chrono::high_resolution_clock::now();

    if (opt->file_type)
        graph = graph_retrival(opt);
    else 
        graph = graph_creation(opt);

    auto end = std::chrono::high_resolution_clock::now();

    if(graph == NULL)
        return -1;
     
    if(opt->printing == 'f')
        cout << "Graph completed!" << endl;

    std::chrono::duration<double> duration = end - start;
    std::cout << "Graph time: " << duration.count() << " seconds" << std::endl;

    if(opt->query_count != 0)
    {
        Stats stats;

        // Calculate Recall rates when provided with a groundtruth //
        if (opt->truth_filename.compare("") != 0) {
            stats = perform_queries_with_accuracy(graph, opt);
            if (stats == NULL)
                return -1;

            cout << "Queries performed: " << opt->query_count << endl;
            cout << "Average overall recall: " << stats->all_avg/opt->query_count << "%" << endl;
            if(stats->unfilt_count != 0)
                cout << "Average type 0 recall: " << stats->unfilt_avg/stats->unfilt_count << "%" << " (from " << stats->unfilt_count << ")" << endl;
            if(stats->filt_count != 0)
                cout << "Average type 1 recall: " << stats->filt_avg/stats->filt_count << "%" << " (from " << stats->filt_count << ")" << endl;
        }
        // Execute without recall calculation when not provided with a groundtruth //
        else
        {
            stats = perform_queries_without_accuracy(graph, opt);
            if (stats == NULL)
                return -1;
        }

        cout << "Total time: " << stats->total_time << endl;
        cout << "Average time per query: " << stats->avg_query_time << endl;

        delete stats;
    }
    destroy_graph(graph);

    delete opt;
}


Graph graph_creation(Options opt)
{
    Graph graph;
    string graph_file;
    if(opt->file_type == 'f')
    {
        if (opt->printing == 'f')
            cout << "Creating Filtered Vamana..." << endl;

        if (create_filtered_vamana_index(&graph, opt->data_filename, opt->L, opt->R, opt->a, opt->dim, opt->rand_init)) {
            cout << "Error creating filtered vamana" << endl;
            delete opt;
            return NULL;
        }
        graph_file = "./data/filtered-graph.bin";
    }
    else if(opt->file_type == 's')
    {
        if (opt->printing == 'f')
            cout << "Creating Stitched Vamana..." << endl;

        graph = create_stiched_vamana_index(opt->data_filename, 'f', opt->L, opt->R, opt->R, opt->a, opt->dim, opt->rand_init);
        if(graph == NULL)
        {
            cout << "Error creating filtered vamana" << endl;
            delete opt;
            return NULL;
        }
        graph_file = "./data/stitched-graph.bin";
    }
    else
    {
        if (opt->printing == 'f')
            cout << "Creating Vamana..." << endl;

        int medoid_pos;
        if (create_vamana_index(&graph, opt->data_filename, opt->L, opt->R, opt->a, medoid_pos, opt->dim, opt->rand_medoid)) {
            cout << "Error creating filtered vamana" << endl;
            delete opt;
            return NULL;
        }
        string graph_file = "./data/unfiltered-graph.bin";
    }

    if (opt->savegraph) 
    {
        ofstream file(graph_file , ios::binary);
        if (!file) {
            cerr << "Error opening file: " << graph_file << " for reading" << endl;
            return NULL;
        }
        // Save graph
        saveGraph(graph, file);
        // Close file
        file.close();
    }

    return graph;
}

Graph graph_retrival(Options opt)
{
    Graph graph;
    if (opt->file_type) 
    {
        graph = create_graph('f', 0, 0);
        // Open graph file
        ifstream file(opt->data_filename, ios::binary);
        if (!file) {
            cerr << "Error opening file: " << opt->data_filename << " for reading" << endl;
            return NULL;
        }
        readGraph(graph, file);
        // CLose file
        file.close();
    }

    return graph;
}

Stats perform_queries_with_accuracy(Graph graph, Options opt)
{
    vector<vector<uint32_t>> groundtruth;
    // IF NOT CREATED -> Create groundtruth file
    if (!fileExists(opt->truth_filename)) {
        // Graph case
        if (opt->file_type) {
            cerr << "Groundtruth can be only created with dataset, not with graph file.." << endl;      // TODO: Check options 
            return NULL;
        }
        create_groundtruth_file(opt->data_filename, opt->queries_filename, opt->truth_filename);
    }
    readKNN(opt->truth_filename, opt->dim, groundtruth);     // Read groundtruth file
    srand(static_cast<unsigned int>(time(0)));


    Stats st = new stats();

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < opt->query_count; i++) {
        set<Candidate, CandidateComparator>* neighbors = new set<Candidate, CandidateComparator>();
        set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();
        int query_pos, query_type;
        Node query = ask_query(opt->queries_filename, query_type, graph->dimensions, query_pos);

        // === filtered vamana search === //
        if (opt->index_type != 'u')
        {
            while(query_type == 2 || query_type == 3)
            {
                destroy_node(query);
                query = ask_query(opt->queries_filename, query_type, 100, query_pos);
            }
            if(query_type == 0)
            {
                query->categories.clear();
                query->categories.insert(graph->all_categories.begin(), graph->all_categories.end());
            }
            int s_count = query->categories.size();
            Node* S = (Node *)malloc(sizeof(*S)*s_count);
            int j = 0;
            for (const int& val : query->categories) {
                S[j] = graph->nodes[graph->medoid_mapping[val]];
                j++;
            }
            filtered_gready_search(graph, S, s_count, query, opt->k, opt->L, query->categories, neighbors, visited);
            free(S);
        }
        //=== Clasic vamana search ===//
        else
        {
            while(query_type != 0)
            {
                destroy_node(query);
                query = ask_query(opt->queries_filename, query_type, 100, query_pos);
            }

            gready_search(graph, graph->nodes[graph->unfiltered_medoid], query, opt->k, opt->L, neighbors, visited);

        }
        //=== Results Printing ===//
        set<int> algorithm_results;
        int j = 0;
        for (const auto& r : *neighbors) {
            if (opt->k == j)
            break;
            algorithm_results.insert(r->to->pos);
            j++;
        }
        set<int> true_results(groundtruth[query_pos].begin(), groundtruth[query_pos].begin() + opt->k);
        
        set<int> intersection;
        set_intersection(algorithm_results.begin(), algorithm_results.end(),
                        true_results.begin(), true_results.end(),
                        inserter(intersection, intersection.begin()));
        
        double recall = static_cast<double>(intersection.size()) / true_results.size();
        j = 0;
        
        if(opt->printing != 'n'){
            for (const auto& r : *neighbors) {
                cout << "Node: " << r->to->pos << " with distance: " << r->distance << endl;
                j++;
            }
        }
        if(opt->printing != 'n'){
            cout << "Query with position: " << query_pos << " -> Recall: " << recall * 100 << "%" << endl;
            cout << "Query type : " << query_type << endl;
            cout << "##########################" << endl << endl;    
        }
        
        if(query_type == 1)
        {
            st->filt_avg += recall;
            st->filt_count++;
        }
        else
        {
            st->unfilt_avg += recall;
            st->unfilt_count++;
        }

        st->all_avg += recall;

        for (const auto& r : *neighbors)
            free(r);
        
        delete neighbors;
        
        for (const auto& r : *visited)
            free(r);
        
        delete visited;

        destroy_node(query);

    }

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration = end - start;
    
    st->all_avg = st->all_avg * 100;
    st->filt_avg = st->filt_avg * 100;
    st->unfilt_avg = st->unfilt_avg * 100;

    st->total_time = duration.count();
    st->avg_query_time = duration.count()/opt->query_count;

    return st;
}


Stats perform_queries_without_accuracy(Graph graph, Options opt)
{
    srand(static_cast<unsigned int>(time(0)));

    Stats st = new stats();

    auto start = std::chrono::high_resolution_clock::now();



    for (int i = 0; i < opt->query_count; i++) {
        set<Candidate, CandidateComparator>* neighbors = new set<Candidate, CandidateComparator>();
        set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();
        int query_pos, query_type;
        Node query = ask_query(opt->queries_filename, query_type, graph->dimensions, query_pos);

        // === filtered vamana search === //
        if (opt->index_type == 'f')
        {
            while(query_type == 2 || query_type == 3)
            {
                destroy_node(query);
                query = ask_query(opt->queries_filename, query_type, 100, query_pos);
            }

            if(query_type == 0)
            {
                query->categories.clear();
                query->categories.insert(graph->all_categories.begin(), graph->all_categories.end());
            }
            int s_count = query->categories.size();
            Node* S = (Node *)malloc(sizeof(*S)*s_count);
            int j = 0;
            for (const int& val : query->categories) {
                S[j] = graph->nodes[graph->medoid_mapping[val]];
                j++;
            }
            filtered_gready_search(graph, S, s_count, query, opt->k, opt->L, query->categories, neighbors, visited);
            free(S);
        }
        //=== Clasic vamana search ===//
        else
        {
            while(query_type != 0)
            {
                destroy_node(query);
                query = ask_query(opt->queries_filename, query_type, 100, query_pos);
            }

            gready_search(graph, graph->nodes[graph->unfiltered_medoid], query, opt->k, opt->L, neighbors, visited);

        }
        // Results
        set<int> algorithm_results;
        int j = 0;
        for (const auto& r : *neighbors) {
            if (opt->k == j)
            break;
            algorithm_results.insert(r->to->pos);
            j++;
        }

        if(opt->printing == 'f')
        {
            j = 0;
            for (const auto& r : *neighbors) {
                cout << "Node: " << r->to->pos << " with distance: " << r->distance << endl;
                j++;
            }
        }
        
        if(opt->printing != 'n')
        {
            cout << "Query with position: " << query_pos << endl;
            cout << "Query type : " << query_type << endl;
            cout << "##########################" << endl << endl;    
        }
        
        for (const auto& r : *neighbors)
            free(r);
        
        delete neighbors;
        
        for (const auto& r : *visited)
            free(r);
        
        delete visited;

        destroy_node(query);

    }

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration = end - start;

    st->total_time = duration.count();
    st->avg_query_time = duration.count()/opt->query_count;

    return st;

}