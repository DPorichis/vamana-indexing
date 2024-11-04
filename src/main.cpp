#include <iostream>
using namespace std;


int main(int argc, char* argv[]) {
    
    string filename;
    string queriesname;
    int k;
    int dimensions;
    float a;
    char type;

    if(argc != 6)
        cerr << "Incorrect arguments. Usage ./bin/project [filename] [data type] [k parameter] [dimensions-parameter] [a-parameter] [queriesfile]" << endl;

    //**** Read arguments ****//

    // Retrive type
    if(argv[2][0] != 'c' && argv[2][0] != 'i' && argv[2][0] != 'f')
    {
        cout << "Argument passed as type (" << argv[2] << ") is not a valid option. (Available options: i, f, c)" << endl;
        cout << "Reminder: Usage ./bin/project [filename] [k parameter] [dimensions-parameter] [a-parameter] [queriesfile]" << endl;
        return -1;
    }
    type = argv[2][0];

    // Retrive k  
    try {
        k = stoi(argv[3]);
    } catch (const std::invalid_argument&) {
        cout << "Argument passed as k (" << argv[3] << ") is not a valid integer." << endl;
        cout << "Reminder: Usage ./bin/project [filename] [k parameter] [dimensions-parameter] [a-parameter] [queriesfile]" << endl;
        return -1;
    } catch (const std::out_of_range&) {
        cout << "Argument passed as k (" << argv[3] << ") is out of range for an integer." << endl;
        return -1;
    }

    // Retrive dimensions
    try {
        dimensions = stoi(argv[4]);
    } catch (const std::invalid_argument&) {
        cout << "Argument passed as dimensions (" << argv[4] << ") is not a valid integer." << endl;
        cout << "Reminder: Usage ./bin/project [filename] [k parameter] [dimensions-parameter] [a-parameter] [queriesfile]" << endl;
        return -1;
    } catch (const std::out_of_range&) {
        cout << "Argument passed as dimensions (" << argv[4] << ") is out of range for an integer." << endl;
        return -1;
    }

    // Retrive a
    try {
        float a = std::stof(argv[5]);
    } catch (const std::invalid_argument&) {
        cout << "Argument passed as a (" << argv[5] << ") is not a valid float." << endl;
        cout << "Reminder: Usage ./bin/project [filename] [k parameter] [dimensions-parameter] [a-parameter] [queriesfile]" << endl;
        return -1;
    } catch (const std::out_of_range&) {
        cout << "Argument passed as a (" << argv[5] << ") is out of range for a float." << endl;
        return -1;
    }

    //***** Gerasime do your thing *****//

    // Create the Vamana Index

    // Ready for queries

    //


}
