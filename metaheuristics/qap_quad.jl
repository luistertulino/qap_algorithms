using JuMP, Gurobi;

n = 12;

#D = zeros(UInt64, n, n);
#F = zeros(UInt64, n, n);
#rng = MersenneTwister(123456789);
#rand!(rng, D); rand!(rng, F);

D = [ 0 90 10 23 43 0 0 0 0 0 0 0;
	   90 0 0 0 0 88 0 0 0 0 0 0;
	   10 0 0 0 0 0 26 16 0 0 0 0;
	   23 0 0 0 0 0 0 0 0 0 0 0;
	   43 0 0 0 0 0 0 0 0 0 0 0;
	 0 88 0 0 0 0 0 0 1 0 0 0;
	 0 0 26 0 0 0 0 0 0 0 0 0;
	 0 0 16 0 0 0 0 0 0 96 0 0;
	 0 0 0 0 0 1 0 0 0 0 29 0;
	 0 0 0 0 0 0 0 96 0 0 0 37;
	 0 0 0 0 0 0 0 0 29 0 0 0;
	 0 0 0 0 0 0 0 0 0 37 0 0 ];

F = [ 0 36 54 26 59 72  9 34 79 17   46 95;
   	  36  0 73 35 90 58 30 78 35 44   79 36;
      54 73  0 21 10 97 58 66 69 61   54 63;
      26 35 21  0 93 12 46 40 37 48   68 85;
      59 90 10 93  0 64  5 29 76 16 5 76;
      72 58 97 12 64  0 96 55 38 54 0 34;
      9 30 58 46  5 96  0 83 35 11   56 37;
      34 78 66 40 29 55 83  0 44 12   15 80;
      79 35 69 37 76 38 35 44  0 64   39 33;
      17 44 61 48 16 54 11 12 64  0   70 86;
      46 79 54 68  5  0 56 15 39 70 0 18;
      95 36 63 85 76 34 37 80 33 86   18  0 ];

m = Model(solver=GurobiSolver(TimeLimit=5.0));
@variable(m, x[1:n,1:n], category = :Bin);
for i in 1:n
	@constraint(m, sum(x[i,j] for j in 1:n) == 1)
end
for j in 1:n
	@constraint(m, sum(x[i,j] for i in 1:n) == 1)
end

@expression(m, obj, begin
	sum( sum( sum( sum( F[i,j]*D[k,l]*x[i,k]*x[j,l] for l in 1:n) for k in 1:n)  for j in 1:n) for i in 1:n)
end);

@objective(m, Min, obj);

status = solve(m);

obj = getobjectivevalue(m); bound = getobjectivebound(m);
gap = 100 * abs(bound - obj)/abs(obj);
gap = ceil(gap,4);
println("Objective value: ", getobjectivevalue(m));
println("Gap: ", gap);