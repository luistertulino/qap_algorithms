#!/bin/bash 

instances="../instances/bur26b.dat ../instances/bur26c.dat ../instances/bur26d.dat ../instances/bur26e.dat 
../instances/bur26f.dat ../instances/bur26g.dat ../instances/bur26h.dat ../instances/els19.dat ../instances/esc16b.dat 
../instances/esc16c.dat ../instances/esc16d.dat ../instances/esc16e.dat ../instances/esc16f.dat ../instances/esc16g.dat 
../instances/esc16h.dat ../instances/esc16i.dat ../instances/esc16j.dat ../instances/esc32b.dat ../instances/esc32c.dat 
../instances/esc32d.dat ../instances/esc32e.dat ../instances/esc32g.dat ../instances/esc32h.dat ../instances/esc64a.dat 
../instances/esc128.dat ../instances/had12.dat ../instances/had14.dat ../instances/had18.dat ../instances/had20.dat 
../instances/kra30b.dat ../instances/kra32.dat ../instances/lipa20b.dat ../instances/lipa30b.dat ../instances/lipa40b.dat 
../instances/lipa50b.dat ../instances/lipa60a.dat ../instances/lipa60b.dat ../instances/lipa70b.dat ../instances/lipa80a.dat 
../instances/lipa80b.dat ../instances/lipa90a.dat ../instances/lipa90b.dat ../instances/nug12.dat ../instances/nug14.dat 
../instances/nug15.dat ../instances/nug16b.dat ../instances/nug17.dat ../instances/nug18.dat ../instances/nug20.dat 
../instances/nug21.dat ../instances/nug22.dat ../instances/nug24.dat ../instances/nug25.dat ../instances/nug27.dat 
../instances/nug28.dat ../instances/nug30.dat ../instances/rou12.dat ../instances/rou15.dat ../instances/rou20.dat 
../instances/scr12.dat ../instances/scr15.dat ../instances/scr20.dat ../instances/sko42.dat ../instances/sko49.dat 
../instances/sko56.dat ../instances/sko64.dat ../instances/sko72.dat ../instances/sko81.dat ../instances/sko90.dat 
../instances/sko100b.dat ../instances/sko100c.dat ../instances/sko100d.dat ../instances/sko100e.dat ../instances/sko100f.dat 
../instances/ste36a.dat ../instances/ste36b.dat ../instances/ste36c.dat ../instances/tai12a.dat 
../instances/tai12b.dat ../instances/tai17a.dat ../instances/tai20b.dat ../instances/tai25a.dat 
../instances/tai25b.dat ../instances/tai30b.dat ../instances/tai35a.dat ../instances/tai35b.dat ../instances/tai40b.dat 
../instances/tai50b.dat ../instances/tai60b.dat ../instances/tai64c.dat ../instances/tai80b.dat ../instances/tai100b.dat 
../instances/tai150b.dat ../instances/tai256c.dat"

min_tabu=17
delta=23
max_fails=167
threshold=56
aspiration=62
#max_iter=1000000

for max_iter in 50000;
do
    for inst in $instances; do
	for (( i = 0; i < 30; i++ )); do
		./exec $inst $min_tabu $delta $max_fails $threshold $aspiration $max_iter
	done
	echo "done all of 30 of ${inst}"
    done
done

python sendemail.py
