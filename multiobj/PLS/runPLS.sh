for n in 20 30 40 50 
do	
	for c in -0.80 0.00
	do
		for ov in 0.0 0.5
		do			
			for (( i = 1; i <= 30; i++ )); do
				./exec reallike/Ter-rl-$n-2-$c-$ov-1.in
				./exec reallike/Ter-rl-$n-3-$c-$c-$ov-1.in
				./exec reallike/Ter-rl-$n-4-$c-$c-$c-$ov-1.in
			done
		done
	done
done
