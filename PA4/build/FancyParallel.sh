#!/bin/bash

#SBATCH --time=00:07:00

totalNums=540
while [ $totalNums -le  5400 ];
do
    srun -n 9 -N 2 Parallel Parallel.out $totalNums >> Timings.txt
    let totalNums=totalNums+180
done
