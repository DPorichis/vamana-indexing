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
