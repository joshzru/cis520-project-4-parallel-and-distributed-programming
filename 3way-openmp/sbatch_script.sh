#!/bin/bash
## Script for running project 4 in Operating System - OpenMP

## SBATCH variables
#SBATCH --job-name=openmp-test
#SBATCH --nodes=2
#SBATCH --cpus-per-task=16
#SBATCH --mem-per-cpu=128M
#SBATCH --time=00:05:00
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jazrubek@ksu.edu
#SBATCH --constraint=moles
#SBATCH --partition=killable.q

## Program variables
NODES=2
THREADS=16
MEMORY="128M"
PROGRAM_NAME="3way-openmp"
PROGRAM_PATH="${HOME}/cis520-project-4-parallel-and-distributed-programming/${PROGRAM_NAME}/build/${PROGRAM_NAME}"
LOG_FILENAME="${NODES}x${THREADS}x${MEMORY}_${PROGRAM_NAME}_log.txt"
CSV_FILENAME="${NODES}x${THREADS}x${MEMORY}_${PROGRAM_NAME}.csv"
OUTPUT_FILENAME="${NODES}x${THREADS}x${MEMORY}_${PROGRAM_NAME}_output.txt"

cp $PROGRAM_PATH $TMPDIR
pushd $TMPDIR

echo "Running ${PROGRAM_NAME} with ${MEMORY} memory, ${THREADS} thread(s), and ${NODES} node(s)" >> $LOG_FILENAME

echo "Runtime (s),Max Memory (KB),CPU Usage (%)" >> $CSV_FILENAME

for i in {0..9}; do
    /usr/bin/time --format="%e,%M,%P" ./$PROGRAM_NAME $THREADS >> $OUTPUT_FILENAME 2>> $CSV_FILENAME
done

kstat --table-cpu-usage-1min "$SLURM_JOB_ID" >> $LOG_FILENAME

popd
mkdir -p $HOME/$PROGRAM_NAME/logs/
mkdir -p $HOME/$PROGRAM_NAME/output/
mkdir -p $HOME/$PROGRAM_NAME/csv/
cp $TMPDIR/$OUTPUT_FILENAME $HOME/$PROGRAM_NAME/output/$OUTPUT_FILENAME
cp $TMPDIR/$LOG_FILENAME $HOME/$PROGRAM_NAME/logs/$LOG_FILENAME
cp $TMPDIR/$CSV_FILENAME $HOME/$PROGRAM_NAME/csv/$CSV_FILENAME
