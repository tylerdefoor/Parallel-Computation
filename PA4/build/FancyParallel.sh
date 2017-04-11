#!/bin/bash

#SBATCH --time=00:05:00
#SBATCH -N 2

totalNums=100000
while [ $totalNums -le 1000000 ];
do
    node=2
    while [ $node -le 16 ];
    do
        srun -n $node Dynamic $totalNums >> Dynamic.txt
        echo "$node"
        let node=node+2
    done
    let totalNums=totalNums+50000
done
