tlimit = 80000.0;

function qap_lin_model(n::Int64, F::Array{Int64, 2}, D::Array{Int64, 2})
    m = Model(solver=GurobiSolver(TimeLimit=tlimit));
    @variable(m, x[1:n,1:n], category = :Bin);

    for i in 1:n
        @constraint(m, sum(x[i,j] for j in 1:n) == 1);
    end
    for j in 1:n
        @constraint(m, sum(x[i,j] for i in 1:n) == 1);
    end

    # Variables and constrains respect to linearization
    @variable(m, y[1:n,1:n,1:n,1:n], category = :Bin);
    @expression(m, sumnew, begin
        sum( sum( sum( sum( y[i,j,k,l] for l in 1:n) for k in 1:n)  for j in 1:n) for i in 1:n)
    end);
    @constraint(m, sumnew == n*n);
    for i = 1:n, j = 1:n, k = 1:n, l = 1:n
        @constraint(m, x[i,j] + x[k,l] - 2*y[i,j,k,l] >= 0);
    end
    
    @expression(m, f, begin
        sum( sum( sum( sum( F[i,j]*D[k,l]*y[i,j,k,l] for l in 1:n) for k in 1:n)  for j in 1:n) for i in 1:n)
    end);

    @objective(m, Min, f);

    status = solve(m);

    obj = getobjectivevalue(m); bound = getobjectivebound(m);
    gap = 100 * abs(bound - obj)/abs(obj);
    gap = ceil(gap,4);
    time = round(Int64, getsolvetime(m));
    
    return (obj, gap, time);
end