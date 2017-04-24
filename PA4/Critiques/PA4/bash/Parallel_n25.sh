#!/bin/bash

#SBATCH -N4
#SBATCH -n25
#SBATCH --output=parallel.out

srun ../exec/parallel $1 $2 $3
