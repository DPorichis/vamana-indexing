# Dit Project 2024-2025
````Note: The experiments in this repository are outdated, you can safely ignore them :)````
### Team members
- Gerasimos Bekos (sdi2100113)
- Dimitrios Stefanos Porichis (sdi2100159)

### Work Distribution

#### Gerasimos Bekos
- Part 1
	- I/O of the program
	- Dataset Management
	- Vamana Index
- Part 2
	- Storing and loading graph representation
	- Groundtruth structure
	- File management
- Part 3
	- Experiments Execution
	- Result Plotting
	- Data management
#### Dimitrios Stefanos Porichis 
- Part 1
	- Graph Representation and relevant functions
	- Greedy Search
	- Robust Pruning
- Part 2
	- Filtered Vamana implementations
	- Stitched Vamana implementations
	- Configuration file and arguments reading
- Part 3
	- Optimization Implementation
	- Report Writting
	- Experiments Scripting

Each person was responsible for creating tests on their part.

### Quick start guide

This project supports 3 Vamana indexing implementations:
- Original Vamana
- Filtered Vamana
- Stitched Vamana


#### Compilation

This project can be compiled by using the `make all` command in the base folder. 

#### Execution

You can run this project by executing `./bin/project ...` from the base folder, followed by the arguments as shown below:
- `data=[filename]`: file containing the data points/graph representation
- `datatype=[f/c/i]`: type of data in file (Defaults to f)
- `filetype=[d/g]`: d for raw-data, g for graph representation (Defaults to d)
- `dimensions=[int >= 1]`: Sets the dimension-count of the elements (Defaults to 100)
- `index=[f/s/u]`: f for filtered Vamana, s for stitched Vamana, and u for unfiltered
- `k=[int >= 1]`: (Defaults to 1)
- `R=[int >= 1]`: (Defaults to 1)
- `L=[int >= 1]`: (Defaults to 1)
- `a=[float >= 1]`: (Defaults to 1)
- `queries=[filename]`: file containing the queries (empty supported only when queries count is set to 0)
- `queriescount=[int >= 0]`: number of queries to be performed from file (Defaults to 1)
- `printing=[true/false]`: Enable or disable detailed printing (Defaults to true)
- `savegraph=[true/false]`: Save the graph created to a file (Defaults to false)
- `truthfile=[filename]`: File containing the ground truth. Accuracy will not be calculated when a value is not set
- `optimized=[true/false]`: Selects the implementation to be used, optimized uses parallelism for the calculations (Defaults to false)
- `randinit=[true/false]`: Selects if random neighbours should be added when initializing filtered and stitched vamana (Defaults to false)
- `randmedoid=[no/semi/yes]`: Selects the way to calculate medoid ONLY in original vamana implementation. 
	- `no`: Performs a brute force search
	- `semi`: Performs a brute force search in a 10% random subpart of the dataset
	- `yes`: Picks a medoid at random.
- `threadcount=[int >=1]`: For thread count > 1, parallelism will be used whenever possible. (Defaults to 1)
- `medoidparallel=[int >=1]`: For medoid parallel > 1 and **randmedoid=no**, brute force medoid calculation will be used with *medoidparallel* threads.

An example of execution is : 
`./bin/project data=./data/dummy-data.bin
datatype=f k=20 R=40 L=50 a=1.4
queries=./data/dummy-queries.bin
queriescount=2 index=u`

You can also set the attributes inside a config file and execute like this:

`./bin/project -config ./config.txt`

*The config file must not have extra whitespaces and have only one attribute in each line.*

Comments can be made in a line using the #

Sample of a config:

```
# I am a config file
data=./data/dummy-data.bin
datatype=f
k=20
R=40
L=50
a=1.4
queries=./data/dummy-queries.bin
queriescount=2
printing=true
savegraph=false
```

An already populated configuration file exists in the main directory, and can be used by running `make run`
Additionally, using `make run-timed` you can also see the project's execution time  

#### Unit Testing

You can run all unit tests available with `make run_tests`, or with the bash script located in `/tests/tests.bash` or one at a time using the executables in the `/bin` folder 

*(Note: Due to ENV variables, some tests may not run if your terminal is not in the `/tests` folder `running ../bin/[testname]`)*

You can check for memory leaks using `make run_valgrind_tests`.

Sample data from the provided website have been included to make running easier and can be found in `./data`.

A smaller dataset for testing was developed containing only 1000 points and 100 queries, you can also find it at `./data`, or populate it using the `./test_io`

### Design choices

For the most part, all algorithms were trivial in their implementation given the paper provided. The most notable design choices are about the graph representation of our program, and the structs used.

#### Graph
- a char type identifier
- an int dimensions parameter
- a distance function pointer that is selected specifically for the type used
- a Vector for storing all nodes
- an int pointing to the medoid if the graph is unfiltered (or -1 otherwise)
- a set with all possible categories found in the graph (ignore for unfiltered)
- a medoid mapping category -> starting point (ignore for unfiltered)

#### Nodes
- an int dimensions parameter
- a void pointer to the array containing the point information
- an int containing the position of the node in the graph's vector
- a Set containing all node's neighbors (represented as Links), sorted by the memory address they point to (no duplicates allowed)
- an int containing the category of the node (previously was a set, optimized to reduce execution times).


#### Links or Candidates 
- a double containing the distance between the nodes;
- a Node pointer to the destination node
	The node of origin contains the Link in its neighbor set.

Candidates are an **alias of Links** and are used to create "candidates" of Links (e.g., in GreadySearch, Prunning, etc.). There is no difference under the hood.

#### On the other hand, Candidate sets differ from Link sets in the way they sort data. 

Candidate sets use the stored distance to order themselves (from closest to farthest), and the "to" attribute to recognize duplicates. 

This sorting makes finding the min in our algorithm faster while maintaining the sort comes at a price of logn for each entry, which we believe is a fair trade.

#### Multiple index support

To make things easier we use the same graph representation for all Indexes, by overloading functions with more fields where relevant.

For example:

````
// Used for unfiltered Vamana
Node add_node_graph(Graph g, int d_count, void* components, int pos);

// Used for filtered Vamana
Node add_node_graph(Graph g, int d_count, void* components, int pos, set<int> categories);
````

Here the node structure remains the same, but specific fields of it will be nulled.

This helped in leveraging the old code we had developed in part 1.

Stitched Vamana is just a map of simple Vamana Indexes, keyed by their category.

#### Parallelism Support
Parallelism is supported in the graph creation of all Vamana versions
- Classic Vamana supports parallel medoid calculation
- Stitched Vamana supports parallel sub-graph creation
- Filtered Vamana supports parallel graph creation, by splitting different nodes to different threads.

Parallelism is supported in the query making process of all graphs. Choosing so splits the number of queries to n sub-threads to execute them concurenetly


#### I/O
For the data insertion, we use a 2D vector that each row contains the node's or the query's information. Each node has 102 values (1 * category + 1 * timestamp + 100 * dimensions), and each query contains 104 values (1 * query_type + 1 * category + 1 * start_timestamp + 1 * end_timestamp + 100 * dimensions).

In order to evaluate the recall of the results, we need a groundtruth file. We calculate the nearest neighbors for each query with bruteforce, based on its type, and then store the results in the file `./data/groundtruth.bin`

 
### Test Report

In our computers, all tests pass without errors and memory leaks. The same applies to the project execution

`vamana_test` and `io_test` are mainly written for memory leak debugging.
