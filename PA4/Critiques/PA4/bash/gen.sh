#!/bin/bash

#SBATCH --output=gen.out

for N in "$@"
do
  srun ../exec/generator $N
done
