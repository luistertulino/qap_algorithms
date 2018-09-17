using JuMP, Gurobi;

include("parsefile.jl");
include("qap_model.jl");

#files = ["rou20.dat" "els19.dat" "had12.dat" "had14.dat" "bur26a.dat" "bur26b.dat"];

#for f in files
    file = "instances/" * ARGS[1];

    (n, F, D) = parsefile(file);

    (obj, gap, time) = qap_model(n, F, D);

    open("results.txt", "a") do results
        writedlm(results, [split(ARGS[1], ".dat")[1] obj gap time], " ");
    end
#end


