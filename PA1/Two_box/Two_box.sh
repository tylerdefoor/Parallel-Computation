#!/bin/bash
#SBATCH -n 2
#SBATCH -N 2
#SBATCH --mem=2048MB
#SBATCH --time=00:05:00
#SBATCH --mail-user=tylerleedefoor@gmail.com
#SBATCH --mail-type=ALL
#SBATCH --output=Two_box.txt

srun Two_box
