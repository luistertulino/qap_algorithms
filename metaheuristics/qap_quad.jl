using JuMP, Gurobi;

D = zeros(UInt64, 4, 4);
F = zeros(UInt64, 4, 4);
rng = MersenneTwister(123456789);
rand!(rng, D); rand!(rng, F);

m = Model(solver=GurobiSolver());
@variable(m, x[1:4,1:4], category = :Bin);
for i in 1:4
	@constraint(m, sum(x[i,j] for j in 1:4) == 1)
end
for j in 1:4
	@constraint(m, sum(x[i,j] for i in 1:4) == 1)
end

@expression(m, obj, begin
	sum( sum( sum( sum( F[i,j]*D[k,l]*x[i,k]*x[j,l] for l in 1:4) for k in 1:4)  for j in 1:4) for i in 1:4)
end);

@objective(m, Min, obj);

