#include <iostream>
#include <algorithm>
#include <set>
#include "vamana.h"
#include "filtered-vamana.h"
#include "vamana-utils.h"
#include <fstream>

using namespace std;

bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}


int main(int argc, char* argv[]) {

    Options opt = new options();
    int dimensions;
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

    vector<vector<uint32_t>> groundtruth;
    dimensions = 100;
    int medoid_pos;

    double filt_avg = 0;
    double unfilt_avg = 0;
    double all_avg = 0;
    int filt_count = 0;
    int unfilt_count = 0;
    
    // Create Filtered/Unfiltered Vamana
    Graph graph;
    if (opt->printing == 'f') {
        if (opt->index_type == 'f')
            cout << "Creating Filtered Vamana..." << endl;
        else if (opt->index_type == 's')
            cout << "Creating Stitched Vamana..." << endl;
        else  
            cout << "Creating Vamana..." << endl;    
    }
    if (opt->index_type == 'f' || opt->index_type == 's') {
        // If the file is GRAPH
        if (opt->file_type) {
            graph = create_graph('f', 0, 0);
            // Open graph file
            ifstream file(opt->data_filename, ios::binary);
            if (!file) {
                cerr << "Error opening file: " << opt->data_filename << " for reading" << endl;
                return -1;
            }
            readGraph(graph, file);
            // CLose file
            file.close();
        }
        // If the file is data
        else {
            if(opt->file_type == 'f')
            {
                if (create_filtered_vamana_index(&graph, opt->data_filename, opt->L, opt->R, opt->a, dimensions, opt->rand_init)) {
                    cout << "Error creating filtered vamana" << endl;
                    delete opt;
                    return -1;
                }
                if (opt->savegraph) {
                    // Open graph file
                    string graph_file = "./data/filtered-graph.bin";
                    ofstream file(graph_file , ios::binary);
                    if (!file) {
                        cerr << "Error opening file: " << graph_file << " for reading" << endl;
                        return -1;
                    }
                    // Save graph
                    saveGraph(graph, file);
                    // Close file
                    file.close();
                }
            }
            else
            {
                graph = create_stiched_vamana_index(opt->data_filename, 'f', opt->L, opt->R, opt->R, opt->a, dimensions, opt->rand_init);
                if(graph == NULL)
                {
                    cout << "Error creating filtered vamana" << endl;
                    delete opt;
                    return -1;
                }
                if (opt->savegraph) {
                    // Open graph file
                    string graph_file = "./data/stitched-graph.bin";
                    ofstream file(graph_file , ios::binary);
                    if (!file) {
                        cerr << "Error opening file: " << graph_file << " for reading" << endl;
                        return -1;
                    }
                    // Save graph
                    saveGraph(graph, file);
                    // Close file
                    file.close();
                }
            }
            
        }
    }
    else {
        if (opt->file_type) {
            graph = create_graph('f', 0, 0);
            // Open graph file
            ifstream file(opt->data_filename, ios::binary);
            if (!file) {
                cerr << "Error opening file: " << opt->data_filename << " for reading" << endl;
                return -1;
            }
            readGraph(graph, file);
            // Close file
            file.close();
        }
        // If the file is data
        else {
            if (create_vamana_index(&graph, opt->data_filename, opt->L, opt->R, opt->a, medoid_pos, dimensions)) {
                cout << "Error creating filtered vamana" << endl;
                delete opt;
                return -1;
            }
            if (opt->savegraph) {
                // Open graph file
                string graph_file = "./data/unfiltered-graph.bin";
                ofstream file(graph_file , ios::binary);
                if (!file) {
                    cerr << "Error opening file: " << graph_file << " for reading" << endl;
                    return -1;
                }
                // Save graph
                saveGraph(graph, file);
                // Close file
                file.close();
            }
        }

        
    }
    if(opt->printing == 'f')
        cout << "Graph completed!" << endl;

    // Calculate Recall rates when provided with a groundtruth //
    if (opt->truth_filename.compare("") != 0) {
        // IF NOT CREATED -> Create groundtruth file
        if (!fileExists(opt->truth_filename)) {
            // Graph case
            if (opt->file_type) {
                cerr << "Groundtruth can be only created with dataset, not with graph file.." << endl;      // TODO: Check options 
                return -1;
            }
            create_groundtruth_file(opt->data_filename, opt->queries_filename, opt->truth_filename);
        }

        readKNN(opt->truth_filename, dimensions, groundtruth);     // Read groundtruth file
        
        srand(static_cast<unsigned int>(time(0)));

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
                filt_avg += recall * 100;
                filt_count++;
            }
            else
            {
                unfilt_avg += recall * 100;
                unfilt_count++;
            }

            all_avg += recall* 100;

            for (const auto& r : *neighbors)
                free(r);
            
            delete neighbors;
            
            for (const auto& r : *visited)
                free(r);
            
            delete visited;

            destroy_node(query);

        }

        if(opt->query_count != 0)
        {
            cout << "Queries performed: " << opt->query_count << endl;
            cout << "Average overall recall: " << all_avg/opt->query_count << "%" << endl;
            if(unfilt_count != 0)
                cout << "Average type 0 recall: " << unfilt_avg/unfilt_count << "%" << " (from " << unfilt_count << ")" << endl;
            if(filt_count != 0)
                cout << "Average type 1 recall: " << filt_avg/filt_count << "%" << " (from " << filt_count << ")" << endl;
        }

    }
    // Execute without recall calculation when not provided with a groundtruth //
    else
    {
        srand(static_cast<unsigned int>(time(0)));

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
    }
    destroy_graph(graph);

    delete opt;

    // Create the Vamana Index
    // Graph graph;
    // if(opt->printing)
    //     cout << "Creating vamana graph..." << endl;
    // if (create_vamana_index(&graph, opt->data_filename, opt->L, opt->R, opt->a, medoid_pos, dimensions)) {
    //     cout << "Error creating vamana" << endl;
    //     delete opt;
    //     return -1;
    // }

    // // Ready for queries
    // if(opt->printing)
    //     cout << "Graph completed!" << endl;
   

    // if (opt->truth_filename.compare("") != 0) {
    //     // Vectors for groundtruth data
    //     vector<file_vector_int> vectors = read_int_vectors_from_file(opt->truth_filename);
      
    //     srand(static_cast<unsigned int>(time(0)));
  
    //     for (int i = 0; i < opt->query_count; i++) {
    //         set<Candidate, CandidateComparator>* neighbours = new set<Candidate, CandidateComparator>();
    //         set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();
    //         int query_pos;
    //         Node query = ask_query(opt->queries_filename, graph->type,graph->dimensions, query_pos);
    //         gready_search(graph, graph->nodes[medoid_pos], query, opt->k, opt->L, neighbours, visited);
    //         set<int> algorithm_results;
    //         int j = 0;
    //         for (const auto& r : *neighbours) {
    //             if (opt->k == j)
    //             break;
    //             algorithm_results.insert(r->to->pos);
    //             j++;
    //         }
    //         set<int> true_results(vectors[query_pos].components.begin(), vectors[query_pos].components.begin() + opt->k);
                
    //         set<int> intersection;
    //         set_intersection(algorithm_results.begin(), algorithm_results.end(),
    //                         true_results.begin(), true_results.end(),
    //                         inserter(intersection, intersection.begin()));

    //         double recall = static_cast<double>(intersection.size()) / true_results.size();
    //         j = 0;
    //         for (const auto& r : *neighbours) {
    //             cout << "Node: " << r->to->pos << " with distance: " << r->distance << endl;
    //             j++;
    //         }
    //         cout << "Query with position: " << query_pos << " -> Recall: " << recall * 100 << "%" << endl;
    //         cout << "##########################" << endl << endl;    
            
            
    //         for (const auto& r : *neighbours)
    //             free(r);
            
    //         delete neighbours;
            
    //         for (const auto& r : *visited)
    //             free(r);
            
    //         delete visited;

    //         destroy_node(query);
    //     }
    // } else {
    //     for (int i = 0; i < opt->query_count; i++) {
    //         set<Candidate, CandidateComparator>* neighbours = new set<Candidate, CandidateComparator>();
    //         set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();
    //         int query_pos;
    //         Node query = ask_query(opt->queries_filename, graph->type,graph->dimensions, query_pos);
    //         gready_search(graph, graph->nodes[medoid_pos], query, opt->k, opt->L, neighbours, visited);
    //         set<int> algorithm_results;
    //         int j = 0;
    //         for (const auto& r : *neighbours) {
    //             if (opt->k == j)
    //             break;
    //             algorithm_results.insert(r->to->pos);
    //             j++;
    //         }
        
    //         j = 0;
    //         for (const auto& r : *neighbours) {
    //             cout << "Node: " << r->to->pos << " with distance: " << r->distance << endl;
    //             j++;
    //         }
    //         cout << "##########################" << endl << endl;    
            
            
    //         for (const auto& r : *neighbours)
    //             free(r);
            
    //         delete neighbours;
            
    //         for (const auto& r : *visited)
    //             free(r);
            
    //         delete visited;

    //         destroy_node(query);
    //     }
    // }
    // delete opt;
    // destroy_graph(graph);
}
