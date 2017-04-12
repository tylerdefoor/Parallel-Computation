#!/bin/bash

#SBATCH --time=00:08:00
#SBATCH --mail-user=tylerleedefoor@gmail.com
#SBATCH --mail-type=ALL

totalNums=1000
while [ $totalNums -le 3000 ];
do
    srun -n 1 Sequential $totalNums >> Sequential.txt
    let totalNums=totalNums+100
done
