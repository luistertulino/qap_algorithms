

for (( i = 0; i < 30; i++ )); do
	./exec reallike/Ter-rl-10-2--0.80-0.0-1.in
	./exec reallike/Ter-rl-10-2-0.00-0.0-1.in
	./exec reallike/Ter-rl-10-3--0.80--0.80-0.0-1.in
	./exec reallike/Ter-rl-10-3-0.00-0.00-0.0-1.in
	./exec reallike/Ter-rl-10-4--0.80--0.80--0.80-0.0-1.in
	./exec reallike/Ter-rl-10-4-0.00-0.00-0.00-0.0-1.in
done

