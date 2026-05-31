# Compile Instructions

Each implementation uses cmake to compile, below are instructions on how to do so.

## Navigation

There are three directories at root: 3way-mpi, 3way-openmp, and 3way-pthread

Navigate into the implementation you want to compile

```
cd 3way-<implementation>
```

Then create a `build` directory.

```
mkdir build
```

Now navigate into the `build` directory

```
cd build
```

## Compile With Cmake

Load the cmake module, if necessary

```
module load CMake/3.23.1-GCCcore-11.3.0
```

Then execute cmake

```
cmake -S ..
```

## Executing

The location of the wiki dump text file is hardcoded into each implementation.

### Pthread

The first argument tells the program how many threads to use.

For example,

```
./3way-pthread 4
```

This will make the program create 4 pthreads to process each line.

### OpenMP

The first argument tells the program how many threads to use.

For example,

```
3way-openmp 4
```

This will make the program execute its `#pragam omp parallel for` preprocessor directive using 4 threads

### MPI

To execute this program, you will need to use `mpirun`

It takes no arguments as the number of nodes and cores are expected to be a part of the batch file or are assigned using flags passed to `mpirun`

For example,

```
mpirun ./3way-mpi
```

You can specify the number of processes with the `-np` flag

For example,
```
mpirun -np 4 ./3way-mpi
```

## Batch scripts

Each implementation directory contains a sbatch script that can be used to schedule the program on Beocat.

While in the implementation directory (not the `build` directory), run

```
sbatch sbatch_script.sh
```

Each script is designed to execute the implementation ten times consecutively and then write the output to three separate files in the users home directory.

To change the number of nodes, cores, or amount of memory, you need to manually change the batch script in two places.

First, the `SBATCH` commands, then in the shell's variables, so that it writes to the proper files.

### 3way-<implementation>/output/

This contains the actual output of the programs run, all of them.

Since each program outputs approximately 1,000,000 lines, it should contain 10,000,000 lines from all ten executing.

I recommend deleting these, or removing them from the script if you don't plan on using them.

### 3way-<implementation>/logs/

This contains some log information, not used too much.

### 3way-<implementation>/csv/

This contains the stats from `time` as well as any output from `stderr`.

If no errors occur, this should be in the correct format to be imported as a csv file into any program.