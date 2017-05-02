#!/bin/bash

#SBATCH --time=00:07:00

totalNums=1008
while [ $totalNums -le  6480 ];
do
    srun -n 16 -N 2 Parallel Parallel.out $totalNums >> Timings16.txt
    let totalNums=totalNums+240
done
