#!/bin/bash
#SBATCH -n 9
#SBATCH -N 2
#SBATCH --time=00:05:00
#SBATCH --mail-user=tylerleedefoor@gmail.com
#SBATCH --mail-type=ALL

srun Parallel Parallel.out 9 9A.txt 9B.txt >> Timings.txt
