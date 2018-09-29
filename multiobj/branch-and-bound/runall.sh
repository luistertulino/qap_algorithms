
for n in 10 15 #20 30 40 50 60 70 80 90 100
do
	for k in 2 3 4
	do
		for c in -1.00 -0.80 -0.40 0.00 0.40 0.80 1.00
		do
			for (( i = 1; i <= 3; i++ )); do
				./exec Ter.$n.$k.$c.$i.in
			done
		done
	done
done