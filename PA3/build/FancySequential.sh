#!/bin/bash

#SBATCH --time=00:05:00

totalNums=50000
while [ $totalNums -le 1000000 ];
do
    srun -n 1 Sequential $totalNums >> Sequential.txt
    let totalNums=totalNums+50000
done
