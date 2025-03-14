all : 
	g++ -std=c++11 -O3 -g -w main.cpp -o ILP -I$$CPLEX_STUDIO_DIR/cplex/include -I$$CPLEX_STUDIO_DIR/concert/include -L$$CPLEX_STUDIO_DIR/cplex/lib/x86-64_linux/static_pic -L$$CPLEX_STUDIO_DIR/concert/lib/x86-64_linux/static_pic -lconcert -lilocplex -lcplex -lm -pthread

clean:
	rm ILP