## Code for directed $(k,\ell)$-plex.

Written by Shuohao Gao in HITSZ.


# Graph data format:
first line: n m
next m lines: a b
Note that vertices are 0~n-1

# Usage
### DPHeuris
compile command need at least c++11:
```
g++ -std=c++11 -O3 -w MDP-pre.cpp -o MDP-pre
./MDP-pre input_graph_file_path k l
```
### DPBB
```
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