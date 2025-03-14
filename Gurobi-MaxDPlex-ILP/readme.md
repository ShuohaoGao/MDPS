### CPLEX for MDPS
- Integer Linear Programming, ILP


### Usage
environment variable `$GUROBI_HOME` is needed, e.g.:
```sh
export GUROBI_HOME="/home/gsh/code/cpp/gurobi950/linux64"
```

```
ls /home/gsh/code/cpp/gurobi950/linux64                                                                        
bin  docs  EULA.pdf  examples  include  lib  matlab  R  ReleaseNotes.html  setup.py  src
```

1. **Compile**:
```sh
g++ main.cpp -o ILP -I${GUROBI_HOME}/include  -L${GUROBI_HOME}/lib  -lgurobi_c++ -lgurobi95 -lpthread -ldl -O3 -std=c++11 -w -g
```

or you can directly use `makefile`:
```sh
make
```

2. **run**
- Global MaxDPlex
```sh
./ILP reduced_graph_path k l
```

- Query MaxDPlex
```sh
./ILP reduced_graph_path k l query_vertices_path
```

### executable file
- [`ILP`](./ILP) is provided, which can be executed on `Ubuntu 22.04`