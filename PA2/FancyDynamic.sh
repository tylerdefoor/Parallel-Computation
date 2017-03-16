#!/bin/bash

#SBATCH --time=00:05:00
#SBATCH -N 2

height=10000
while [ $height -le 40000 ];
do
    node=2
    while [ $node -le 16 ];
    do
        srun -n $node Dynamic $height $height >> Dynamic.txt
        let node=node+2
    done
    let height=height+5000
done
