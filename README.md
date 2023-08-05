## Code for directed $(k,\ell)$-plex.

Written by *Shuohao Gao* in **HITSZ**.


# Graph data format:
first line: 
```n m```

next $m$ lines are edges: ```a b```

Note that  $0 \leq a,b \leq n-1$ & self-loops and multi-edges are removed.

# Usage

### DPHeuris
compile command need at least c++11:
```
cd DPHeuris
g++ -std=c++11 -O3 -w MDP-pre.cpp -o MDP-pre
./MDP-pre input_graph_file_path k l
```
reduced graph is dumped to ```./reduced_graph/```

### DPBB
```
cd DPBB
g++ -std=c++11 -O3 -w MDP-bb.cpp -o MDP-bb
./MDP-bb reduced_graph_file_path k l
```

# Example
### ./data/graph.txt
```
3 2
0 1
1 2
```
### DPHeuris:
```
./MDP-pre ../data/graph.txt 2 2
```
### DPBB:
```
./MDP-bb ../reduced_graph/graph_K=2_L=2.txt 2 2
```