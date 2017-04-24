#!/bin/bash

#SBATCH -N2
#SBATCH -n16
#SBATCH --output=parallel.out

srun ../exec/parallel $1 $2 $3
