#!/bin/bash

#SBATCH --time=00:07:00

totalNums=1000
while [ $totalNums -le  7600 ];
do
    srun -n 25 -N 4 Parallel Parallel.out $totalNums >> Timings25.txt
    let totalNums=totalNums+300
done
