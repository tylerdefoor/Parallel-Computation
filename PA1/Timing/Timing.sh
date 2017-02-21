#!/bin/bash
#SBATCH -n 2
#SBATCH -N 2
#SBATCH --mem=2048MB
#SBATCH --time=02:00:00
#SBATCH --mail-user=tylerleedefoor@gmail.com
#SBATCH --mail-type=ALL
#SBATCH --output=Timing10.txt

srun Timing
