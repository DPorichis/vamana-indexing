#include <iostream>
#include <algorithm>
#include <set>
#include "vamana.h"
#include "filtered-vamana.h"
#include "vamana-utils.h"

using namespace std;


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
    create_groundtruth_file(opt->data_filename, opt->queries_filename, opt->truth_filename);
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
            if (create_filtered_vamana_index(&graph, opt->data_filename, opt->L, opt->R, opt->a, dimensions)) {
                cout << "Error creating filtered vamana" << endl;
                delete opt;
                return -1;
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

        // In case we have a groundtruth file
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
                if (opt->index_type == 'f') {
                    int s_count = query->categories.size();
                    Node* S = (Node *)malloc(sizeof(*S)*s_count);
                    int j = 0;
                    for (const int& val : query->categories) {
                        S[j] = graph->nodes[graph->medoid_mapping[val]];
                        j++;
                    }
                    filtered_gready_search(graph, S, s_count, query, opt->k, opt->L, query->categories, neighbors, visited);
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



        destroy_graph(graph);
    }
    else if (opt->index_type == 's') {

    }

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
