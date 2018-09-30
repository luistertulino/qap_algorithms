for n in 6 7 8 9 #5 10 15 20 30 40 50 60 70 80 90 100
do
	for k in 2 3 4
	do
		for c in -1 -0.8 -0.4 0 0.4 0.8 1
		do
			for (( i = 1; i <= 3; i++ )); do
				./makeQAPuni -n $n -k $k -c $c -i $i
			done
		done
	done
done
