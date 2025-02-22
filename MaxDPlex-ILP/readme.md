### CPLEX for MDPS
- Integer Linear Programming, ILP


### Usage
environment variable `$CPLEX_STUDIO_DIR` is needed, e.g.:
```sh
export CPLEX_STUDIO_DIR=/home/gsh/cplex
```

```
ls /home/gsh/cplex
README.html  Uninstall  concert  cplex  cpoptimizer  doc  license  opl  python  swidtag
```

1. **Compile**:
```sh
g++ -std=c++11 -O3 -g -w main.cpp -o ILP -I$$CPLEX_STUDIO_DIR/cplex/include -I$$CPLEX_STUDIO_DIR/concert/include -L$$CPLEX_STUDIO_DIR/cplex/lib/x86-64_linux/static_pic -L$$CPLEX_STUDIO_DIR/concert/lib/x86-64_linux/static_pic -lconcert -lilocplex -lcplex -lm -pthread
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