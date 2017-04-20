#!/bin/bash
#SBATCH -n 9
#SBATCH --mem=2048MB
#SBATCH --time=00:05:00
#SBATCH --mail-user=tylerleedefoor@gmail.com
#SBATCH --mail-type=ALL

srun Parallel 1000 >> Parallel.txt
