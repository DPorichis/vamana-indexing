# Dit Project 2024-2025
### Team members
- Gerasimos Bekos (sdi2100113)
- Dimitrios Stefanos Porichis (sdi2100159)

### Work Distribution (Part 1)

#### Gerasimos Mpekos
- I/O of the program
- Dataset Management
- Vamana Index

#### Dimitrios Stefanos Porichis 
- Graph Representation and relevant functions
- Greedy Search
- Robust Pruning

Each person was responsible for creating tests on their part.

### Quick start guide

#### Compilation

This project can be compiled by using the make all command in the base folder. 

#### Execution

You can run this project by executing `./bin/project ...` from the base folder, followed by the arguments as shown below:

`./bin/project [filename] [data type] [k parameter] [a-parameter] [queriesfile]`

If you have a ground truth for your data you will be asked to enter it after the graph creation, via prompt

You will also be asked to select how many queries you want to perform from the file you provided. 

An example execusion is : 

`./bin/project ./data/siftsmall/siftsmall_base.fvecs f 20 1.4 ./data/siftsmall/siftsmall_query.fvecs`


and the groundtruth file is : `./data/siftsmall/siftsmall_groundtruth.ivecs`

#### Unit Testing


You can run all unit tests available with the bash script located in `/tests/tests.bash` or one at a time using the executables in the `/bin` folder 
*(Note: Due to ENV variables, some tests may not run if your terminal is not in the `/tests` folder `running ../bin/[testname]`)*

Sample data from the provided website have been included to make running easier and can be found in `./data`.

### Design choices

For the most part, all algorithms were trivial in their implementation given the paper provided. The most notable design choices are about the graph representation of our program, and the structs used.

#### Graph
- a char type identifier
- an int dimensions parameter
- a distance function pointer that is selected specifically for the type used
- a Vector for storing all nodes

#### Nodes
- an int dimensions parameter
- a void pointer to the array containing the point information
- an int containing the position of the node in the graph's vector
- a Set containing all node's neighbors (represented as Links), sorted by the memory address they point to (no duplicates allowed)

#### Links or Candidates 
- a double containing the distance between the nodes;
- a Node pointer to the destination node
	The node of origin contains the Link in its neighbor set.

Candidates are an **alias of Links** and are used for creating "candidates" of Links (ex. in GreadySearch, Prunning, ...). It doesn't have a difference under the hood.

#### On the other hand, Candidate sets differ from Link sets in the way they sort data. 

Candidate sets use the stored distance to order themselves (from closest to farthest), and the "to" attribute to recognize duplicates. 

This sorting makes finding the min in our algorithm faster while maintaining the sort comes at a price of logn for each entry, which we believe is a fair trade.

#### I/0
For the data insertion we use a struct that contains two elements. The first one called "d" contains the node dimension. The other one is a vector that contains the components of the node. 

### Test Report

In our computers, all tests pass without errors and memory leaks. Same applies for the project execution

For algorithms where the results are dependent on the initial random graph (Vamana, greedy, prune) our test doesn't use hard values for evaluation but rather soft metrics and printing to understand if it's working.
