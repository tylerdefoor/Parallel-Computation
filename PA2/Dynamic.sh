#!/bin/bash
#SBATCH -n 9
#SBATCH --mem=2048MB
#SBATCH --time=00:10:00
#SBATCH --mail-user=tylerleedefoor@gmail.com
#SBATCH --mail-type=ALL
#SBATCH --output=Dynamic.txt

srun Dynamic