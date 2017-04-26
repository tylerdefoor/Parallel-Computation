#!/bin/bash
#SBATCH -n 25
#SBATCH --time=00:05:00
#SBATCH --mail-user=tylerleedefoor@gmail.com
#SBATCH --mail-type=ALL

srun Parallel 1000 >> Parallel.txt
