#!/bin/bash

#SBATCH -n 1
#SBATCH --time=00:10:00
#SBATCH --output=../bin/seq.out

srun sequ m120
