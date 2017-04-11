#!/bin/bash

#SBATCH --time=00:05:00
#SBATCH --mail-user=tylerleedefoor@gmail.com
#SBATCH --mail-type=ALL

totalNums=1000
while [ $totalNums -le 10000 ];
do
    srun -n 1 Sequential $totalNums >> Sequential.txt
    let totalNums=totalNums+1000
done
