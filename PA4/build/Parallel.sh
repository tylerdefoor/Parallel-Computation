#!/bin/bash
#SBATCH -n 9
#SBATCH --time=00:05:00
#SBATCH --mail-user=tylerleedefoor@gmail.com
#SBATCH --mail-type=ALL

srun Parallel 3 >> Parallel.txt
