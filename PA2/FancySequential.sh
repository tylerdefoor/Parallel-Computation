#!/bin/bash

#SBATCH --time=00:05:00

height=10000
while [ $height -le 40000 ];
do
    srun -n 1 Sequential $height $height >> Sequential.txt
    let height=height+5000
done
