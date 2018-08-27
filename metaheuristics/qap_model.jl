using JuMP, Gurobi;

function qap_model(n::Int64, F::Array{Int64, 2}, D::Array{Int64, 2})
    m = Model(solver=GurobiSolver(TimeLimit=5.0));
    @variable(m, x[1:n,1:n], category = :Bin);
    for i in 1:n
        @constraint(m, sum(x[i,j] for j in 1:n) == 1)
    end
    for j in 1:n
        @constraint(m, sum(x[i,j] for i in 1:n) == 1)
    end

    @expression(m, f, begin
        sum( sum( sum( sum( F[i,j]*D[k,l]*x[i,k]*x[j,l] for l in 1:n) for k in 1:n)  for j in 1:n) for i in 1:n)
    end);

    @objective(m, Min, f);

    status = solve(m);

    obj = getobjectivevalue(m); bound = getobjectivebound(m);
    gap = 100 * abs(bound - obj)/abs(obj);
    gap = ceil(gap,4);
    
    return (obj, gap);
end
