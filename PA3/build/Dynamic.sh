#!/bin/bash
#SBATCH -n 8
#SBATCH --mem=2048MB
#SBATCH --time=00:05:00
#SBATCH --mail-user=tylerleedefoor@gmail.com
#SBATCH --mail-type=ALL

srun Dynamic numbers >> Dynamic.txt
