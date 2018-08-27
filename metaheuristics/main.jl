
include("parsefile.jl");
include("qap_model.jl");

(n, F, D) = parsefile(ARGS[1]);

(obj, gap) = qap_model(n, F, D);

open("results.txt", "a") do f
    writedlm(f, [split(args, ".dat")[1] obj gap], " ")
end
