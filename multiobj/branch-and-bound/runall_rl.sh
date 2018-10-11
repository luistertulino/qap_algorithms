for n in 20 
do	
	for c in -0.80 0.00 0.80
	do
		for ov in 0.0 0.5 1.0
		do
			for (( i = 1; i <= 1; i++ )); do
				./exec reallike/Ter-rl-$n-2-$c-$ov-$i.in
				./exec reallike/Ter-rl-$n-3-$c-$c-$ov-$i.in
				./exec reallike/Ter-rl-$n-4-$c-$c-$c-$ov-$i.in
			done
		done
	done
done