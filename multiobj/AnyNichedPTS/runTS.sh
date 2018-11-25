for n in 10 #20 30 40 50 
do	
	for c in -0.80 #0.00
	do
		for ov in 0.0 #0.5
		do			
			for (( i = 1; i <= 1; i++ )); do
				./exec reallike/Ter-rl-$n-2-$c-$ov-1.in $n $((2 * $n / 5)) $((5 * $n)) 0.2
				./exec reallike/Ter-rl-$n-3-$c-$c-$ov-1.in $n $((3 * $n / 5)) $((5 * $n)) 0.2
				./exec reallike/Ter-rl-$n-4-$c-$c-$c-$ov-1.in $n $((4 * $n / 5)) $((5 * $n)) 0.2
			done
		done
	done
done
