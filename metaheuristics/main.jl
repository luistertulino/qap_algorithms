
include("parsefile.jl");
include("qap_model.jl");

files = ["chr15a.dat" "chr15b.dat" "chr18a.dat" "chr20a.dat" "chr25a.dat" "rou12.dat" "rou15.dat" "rou20.dat" "els19.dat" "had12.dat" "had14.dat" "bur26a.dat" "bur26b.dat"];

for f in files
    file = "instances/" * f;

    (n, F, D) = parsefile(file);

    (obj, gap, time) = qap_model(n, F, D);

    open("results.txt", "a") do results
        writedlm(results, [split(f, ".dat")[1] obj gap time], " ");
    end
end


