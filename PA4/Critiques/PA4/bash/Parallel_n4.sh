#!/bin/bash

#SBATCH -N1
#SBATCH -n4
#SBATCH --output=parallel.out

srun ../exec/parallel $1 $2 $3
