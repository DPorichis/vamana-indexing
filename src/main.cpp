#include <iostream>
#include <algorithm>
#include <set>
#include "vamana.h"
#include "filtered-vamana.h"
#include "vamana-utils.h"
#include <filesystem>

using namespace std;

namespace fs = std::filesystem;

bool fileExists(const string& filename) {
    return fs::exists(filename);
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

    if(opt->printing)
        print_options(opt);

    // Files that will be used
    string dataset_file = "../data/dummy-data.bin";
	string queries_file = "../data/dummy-queries.bin";
	string groundtruth_file = "../data/groundtruth.bin";

    vector<vector<uint32_t>> groundtruth;
    dimensions = 100;
    int medoid_pos;
    
    // Create Filtered/Unfiltered Vamana
    if (opt->index_type == 'f' || opt->index_type == 'u') {
        Graph graph;
        if (opt->printing) {
            if (opt->index_type == 'f')
                cout << "Creating Filtered Vamana..." << endl;
            else   
                cout << "Creating Vamana..." << endl;    
        }
        if (opt->index_type == 'f') {
            // if (fileExists("./data/filtered-graph.bin")) {
            //     graph = create_graph('f', 0, 0);
            //     readGraph(graph, "./data/filtered-graph.bin");
            // }
            // else {
            //     if (create_filtered_vamana_index(&graph, opt->data_filename, opt->L, opt->R, opt->a, dimensions)) {
            //         cout << "Error creating filtered vamana" << endl;
            //         delete opt;
            //         return -1;
            //     }
            //     saveGraph(graph, "./data/filtered-graph.bin");
            // }

            // If the file is GRAPH
            if (opt->file_type) {
                graph = create_graph('f', 0, 0);
                readGraph(graph, opt->data_filename);
            }
            // If the file is data
            else {
                if (create_filtered_vamana_index(&graph, opt->data_filename, opt->L, opt->R, opt->a, dimensions)) {
                    cout << "Error creating filtered vamana" << endl;
                    delete opt;
                    return -1;
                }
                if (opt->savegraph) {
                    saveGraph(graph, "./data/filtered-graph.bin");
                }
            }
        }
        else {
            if (create_vamana_index(&graph, opt->data_filename, opt->L, opt->R, opt->a, medoid_pos, dimensions)) {
                cout << "Error creating filtered vamana" << endl;
                delete opt;
                return -1;
            }
        }
        if(opt->printing)
            cout << "Graph completed!" << endl;

        // Calculate Recall rates when provided with a groundtruth //
        if (opt->truth_filename.compare("") != 0) {
            // IF NOT CREATED -> Create groundtruth file
            create_groundtruth_file(opt->data_filename, opt->queries_filename, opt->truth_filename);
            readKNN(opt->truth_filename, dimensions, groundtruth);     // Read groundtruth file

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
                for (const auto& r : *neighbors) {
                    cout << "Node: " << r->to->pos << " with distance: " << r->distance << endl;
                    j++;
                }
                cout << "Query with position: " << query_pos << " -> Recall: " << recall * 100 << "%" << endl;
                cout << "##########################" << endl << endl;    
                
                
                for (const auto& r : *neighbors)
                    free(r);
                
                delete neighbors;
                
                for (const auto& r : *visited)
                    free(r);
                
                delete visited;

                destroy_node(query);

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

                j = 0;
                for (const auto& r : *neighbors) {
                    cout << "Node: " << r->to->pos << " with distance: " << r->distance << endl;
                    j++;
                }

                cout << "Query with position: " << query_pos << endl;
                cout << "##########################" << endl << endl;    
                
                
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
    }
    else if (opt->index_type == 's') {
        map<int,Graph>* index_mapping = create_stiched_vamana_index(opt->data_filename, opt->data_type, opt->L, opt->R, opt->R, opt->a, dimensions);
        if (index_mapping == NULL)
        {
            cout << "Error creating stiched vamana" << endl;
            delete opt;
            return -1;
        }

        cout << "Index is ready" << endl;

        // IF NOT CREATED -> Create groundtruth file
        create_groundtruth_file(opt->data_filename, opt->queries_filename, opt->truth_filename);
        readKNN(opt->truth_filename, dimensions, groundtruth);     // Read groundtruth file

        cout << "Ground truth is ready" << endl;

        srand(static_cast<unsigned int>(time(0)));

        for (int i = 0; i < opt->query_count; i++) {
            int query_pos, query_type;
            
            Node query = ask_query(opt->queries_filename, query_type, 100, query_pos);
            while(query_type == 2 || query_type == 3)
            {
                destroy_node(query);
                query = ask_query(opt->queries_filename, query_type, 100, query_pos);
            }
            
            set<Candidate, CandidateComparator>* total_neighbors = new set<Candidate, CandidateComparator>();
            
            //cout << "Performing Query #" << query_pos << " of type " << query_type << endl;

            if(query_type == 0)
            {
                query->categories.clear();
                for (const auto& pair : *index_mapping)
                    query->categories.insert(pair.first);
            }

            for (const int& val : query->categories) {
                // cout << "Performing gready search on category graph #" << val << endl;
                Graph category_graph = (*index_mapping)[val];
            
                set<Candidate, CandidateComparator>* neighbors = new set<Candidate, CandidateComparator>();
                set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();
            
                gready_search(category_graph, category_graph->nodes[category_graph->unfiltered_medoid], query, opt->k, opt->L, neighbors, visited);
                
                for (auto it = neighbors->begin(); it != neighbors->end(); ++it) {
                    Candidate to_insert = create_candidate_copy(*it);
                    auto result = total_neighbors->insert(to_insert);
                    if (!result.second) {
                        free(to_insert);
                    }
                }

                for (const auto& r : *neighbors)
                    free(r);
                delete neighbors;
                
                for (const auto& r : *visited)
                    free(r);
                delete visited;
            }
            // Results
            set<int> algorithm_results;
            int j = 0;
            for (const auto& r : *total_neighbors) {
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
            for (const auto& r : *total_neighbors) {
                if(opt->k == j)
                    break;
                cout << "Node: " << r->to->pos << " with distance: " << r->distance << endl;
                j++;
            }
            cout << "Query with position: " << query_pos << " -> Recall: " << recall * 100 << "%" << endl;
            cout << "##########################" << endl << endl;    
            
            
            for (const auto& r : *total_neighbors)
                free(r);
            
            delete total_neighbors;
            
            destroy_node(query);

        }

        for (const auto& pair : *index_mapping) {
            destroy_graph(pair.second);
        }
        delete index_mapping;
        
    }
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
