#!/bin/bash

#SBATCH -n 9
#SBATCH --tasks-per-node=8
#SBATCH --time=00:10:00
#SBATCH --output=../bin/parallel.out

srun para m120

