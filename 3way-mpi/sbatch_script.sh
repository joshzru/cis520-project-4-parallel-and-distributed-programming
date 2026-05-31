#!/bin/bash

## SBATCH Variables
#SBATCH --job-name=mpi-testing
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=4
#SBATCH --mem-per-cpu=3G
#SBATCH --time=00:05:00
#SBATCH --partition=killable.q
#SBATCH --constraint=moles
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jazrubek@ksu.edu

## Program Variables
PROGRAM_NAME="3way-mpi"
NODES=2
THREADS=4
MEMORY="3G"

## Output Files
PROGRAM_OUTPUT="${HOME}/${PROGRAM_NAME}/output/${NODES}x${THREADS}x${MEMORY}_${PROGRAM_NAME}_output.txt"
PROGRAM_LOGS="${HOME}/${PROGRAM_NAME}/logs/${NODES}x${THREADS}x${MEMORY}_${PROGRAM_NAME}_logs.txt"
CSV_FILENAME="${HOME}/${PROGRAM_NAME}/csv/${NODES}x${THREADS}x${MEMORY}_${PROGRAM_NAME}.csv"

## Create Output Directories
mkdir -p $HOME/$PROGRAM_NAME/output
mkdir -p $HOME/$PROGRAM_NAME/logs
mkdir -p $HOME/$PROGRAM_NAME/csv

## Load Modules
module reset
module load CMake/3.23.1-GCCcore-11.3.0 foss/2022a OpenMPI/4.1.4-GCC-11.3.0

echo "Runtime (s), Max Memory (KB), CPU Usage (%)" > $CSV_FILENAME

for i in {0..9}; do
        /usr/bin/time --format="%e,%M,%P" mpirun $HOME/cis520-project-4-parallel-and-distributed-programming/$PROGRAM_NAME/build/$PROGRAM_NAME > $PROGRAM_OUTPUT 2>> $CSV_FILENAME
done
