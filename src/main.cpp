#include <iostream>
#include <algorithm>
#include <set>
#include "vamana.h"
using namespace std;


int main(int argc, char* argv[]) {
    int k;
    int dimensions;
    float a;
    char type;

    if(argc != 6)
    {                                       //    0           1            2           3             4             5  
        cerr << "Incorrect arguments. Usage ./bin/project [filename] [data type] [k parameter] [a-parameter] [queriesfile]" << endl;
        return -1;
    }    
    //**** Read arguments ****//

    // Retrive type
    if(argv[2][0] != 'c' && argv[2][0] != 'i' && argv[2][0] != 'f')
    {
        cout << "Argument passed as type (" << argv[2] << ") is not a valid option. (Available options: i, f, c)" << endl;
        cout << "Reminder: Usage ./bin/project [filename] [data type] [k parameter] [a-parameter] [queriesfile]" << endl;
        return -1;
    }
    type = argv[2][0];

    // Retrive k  
    try {
        k = stoi(argv[3]);
        if(k < 1)
        {
            cout << "k must be >= 1 (k passed '" << argv[3] << "')" << endl;
            cout << "Reminder: Usage ./bin/project [filename] [data type] [k parameter] [a-parameter] [queriesfile]" << endl;
            return -1;
        }

    } catch (const std::invalid_argument&) {
        cout << "Argument passed as k (" << argv[3] << ") is not a valid integer." << endl;
        cout << "Reminder: Usage ./bin/project [filename] [data type] [k parameter] [a-parameter] [queriesfile]" << endl;
        return -1;
    } catch (const std::out_of_range&) {
        cout << "Argument passed as k (" << argv[3] << ") is out of range for an integer." << endl;
        return -1;
    }

    // Retrive a
    try {
        a = std::stof(argv[4]);
        if(a < 1)
        {
            cout << "a must be >= 1 (a passed '" << argv[4] << "')" << endl;
            cout << "Reminder: Usage ./bin/project [filename] [data type] [k parameter] [a-parameter] [queriesfile]" << endl;
            return -1;
        }
    } catch (const std::invalid_argument&) {
        cout << "Argument passed as a (" << argv[4] << ") is not a valid float." << endl;
        cout << "Reminder: Usage ./bin/project [filename] [data type] [k parameter] [a-parameter] [queriesfile]" << endl;
        return -1;
    } catch (const std::out_of_range&) {
        cout << "Argument passed as a (" << argv[4] << ") is out of range for a float." << endl;
        return -1;
    }

    // Paths creators
    // Path of folder

    // Path for data_set
    string data_set = argv[1];

    // Path for query_set
    string queries = argv[5];

    // Parameters initialization 
    int L = 80;
    int R = 20;
    int medoid_pos;

    // Create the Vamana Index
    Graph graph;
    cout << "Creating vamana graph..." << endl;
    if (create_vamana_index(&graph, data_set, L, R, a, medoid_pos)) {
        cout << "Error creating vamana" << endl;
        return -1;
    }

    // Ready for queries
    cout << "Graph completed!" << endl;

    // Ask for groundtruth file
    string groundtruth;
    cout << "Give me the groundtruth filename (if it doesn't exist write 'n')" << endl;
    cin >> groundtruth;

    int queries_count;
    cout << "How many queries would you want to check for their " << k << " neighbours?" << endl;
    cin >> queries_count;

   
    if (groundtruth.compare("n") != 0) {
         // Vectors for groundtruth data
        vector<file_vector_int> vectors = read_int_vectors_from_file(groundtruth);
    
        srand(static_cast<unsigned int>(time(0)));
  
        for (int i = 0; i < queries_count; i++) {
            set<Candidate, CandidateComparator>* neighbours = new set<Candidate, CandidateComparator>();
            set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();
            int query_pos;
            Node query = ask_query(queries, graph->type,graph->dimensions, query_pos);
            gready_search(graph, graph->nodes[medoid_pos], query, k, L, neighbours, visited);
            set<int> algorithm_results;
            int j = 0;
            for (const auto& r : *neighbours) {
                if (k == j)
                break;
                algorithm_results.insert(r->to->pos);
                j++;
            }
            set<int> true_results(vectors[query_pos].components.begin(), vectors[query_pos].components.begin() + k);
                
            set<int> intersection;
            set_intersection(algorithm_results.begin(), algorithm_results.end(),
                            true_results.begin(), true_results.end(),
                            inserter(intersection, intersection.begin()));

            double recall = static_cast<double>(intersection.size()) / true_results.size();
            j = 0;
            for (const auto& r : *neighbours) {
                cout << "Node: " << r->to->pos << " with distance: " << r->distance << endl;
                j++;
            }
            cout << "Query with position: " << query_pos << " -> Recall: " << recall * 100 << "%" << endl;
            cout << "##########################" << endl << endl;    
            
            
            for (const auto& r : *neighbours)
                free(r);
            
            delete neighbours;
            
            for (const auto& r : *visited)
                free(r);
            
            delete visited;

            destroy_node(query);
        }
    } else {
        for (int i = 0; i < queries_count; i++) {
            set<Candidate, CandidateComparator>* neighbours = new set<Candidate, CandidateComparator>();
            set<Candidate, CandidateComparator>* visited = new set<Candidate, CandidateComparator>();
            int query_pos;
            Node query = ask_query(queries, graph->type,graph->dimensions, query_pos);
            gready_search(graph, graph->nodes[medoid_pos], query, k, L, neighbours, visited);
            set<int> algorithm_results;
            int j = 0;
            for (const auto& r : *neighbours) {
                if (k == j)
                break;
                algorithm_results.insert(r->to->pos);
                j++;
            }
        
            j = 0;
            for (const auto& r : *neighbours) {
                cout << "Node: " << r->to->pos << " with distance: " << r->distance << endl;
                j++;
            }
            cout << "##########################" << endl << endl;    
            
            
            for (const auto& r : *neighbours)
                free(r);
            
            delete neighbours;
            
            for (const auto& r : *visited)
                free(r);
            
            delete visited;

            destroy_node(query);
        }
    }
    destroy_graph(graph);

}
