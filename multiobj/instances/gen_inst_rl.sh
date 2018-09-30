for n in 5 6 7 8 9 10 15 20 30 40 50 
do
	for k in 2 3 4
	do
		for c in -0.80 0.00 0.80
		do
			for ov in 0 0.5 1
			do
				for (( i = 1; i <= 3; i++ )); do
					./makeQAPrl -n $n -k $k -c1 $c -c2 $c -c3 $c -ov $ov -i $i
				done
			done
		done
	done
done
