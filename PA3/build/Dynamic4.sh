#!/bin/bash

#SBATCH --time=00:05:00
#SBATCH -n 4

totalNums=300000
while [ $totalNums -le 1000000 ];
do
    srun Dynamic $totalNums >> Dynamic.txt
    let totalNums=totalNums+100000
done
