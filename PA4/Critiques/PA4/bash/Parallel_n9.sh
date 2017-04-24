#!/bin/bash

#SBATCH -N2
#SBATCH -n9
#SBATCH --output=parallel.out

srun ../exec/parallel $1 $2 $3
