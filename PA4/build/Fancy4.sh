#!/bin/bash

#SBATCH --time=00:07:00

totalNums=540
while [ $totalNums -le 4100 ];
do
    srun -n 4 -N 1 Parallel Parallel.out $totalNums >> Timings4.txt
    let totalNums=totalNums+120
done
