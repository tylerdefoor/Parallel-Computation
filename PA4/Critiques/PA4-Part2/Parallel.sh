#!/bin/bash

#SBATCH --ntasks=9
#SBATCH --output=Parallel.out
#SBATCH --mem=2048MB
#SBATCH --time=00:10:00
#SBATCH --mail-type=ALL

srun --kill-on-bad-exit=1 parallel 9 1 1000 0
