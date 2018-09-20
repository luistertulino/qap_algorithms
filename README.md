Algorithms for the Quadratic Assignment Problem, developed in Metaheuristics and Multi-objective optimization classes.

Metaheuristics:
  1. A implementation of the Quadratic Programming model using Julia (libraries: JuMP and Gurobi). To run the program:
    - Install Julia 0.6.4
    - On Julia command line, run: `Pkg.add("JuMP")` and `Pkg.add("Gurobi")`
    - You may execute on terminal, using the command `julia main.jl instance`, 
      where 'instance' is one of the instances on `instances`folder
      
  2. A naive heuristic for the problem based on Gilmore-Lawler bound. To run:
    - execute `make` on `libhungarian` folder
    - on `naive` folder, run the `compile.sh` script
    - Run `./exec instance` (type `instance` without the '.dat' extension - see `script.sh` for examples)
