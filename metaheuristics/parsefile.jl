function parsefile(file::AbstractString)
    instance = readdlm(file);
    n = instance[1,1];
    F = instance[2:n+1,1:n];
    D = instance[n+2:2n+1,1:n];
    F = convert(Array{Int64,2}, F);
    D = convert(Array{Int64,2}, D);

    return (n, F, D);
end
