#!/bin/bash

#SBATCH -N5
#SBATCH -n36
#SBATCH --output=parallel.out

srun ../exec/parallel $1 $2 $3
