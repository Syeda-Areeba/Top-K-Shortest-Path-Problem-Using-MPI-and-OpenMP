# MPI-OMP Shortest Path Algorithm

This program implements a parallelized version of Dijkstra's algorithm using both MPI (Message Passing Interface) and OpenMP (Open Multi-Processing) to find the K shortest paths between randomly selected source-sink node pairs in a directed graph.

## Table of Contents

- [Introduction](#introduction)
- [Dependencies](#dependencies)
- [Installation](#installation)
- [Usage](#usage)
- [Algorithm Overview](#algorithm-overview)
- [Performance](#performance)

## Introduction

The code provided is a parallelized implementation of Dijkstra's algorithm for finding the K shortest paths in a directed graph. It uses MPI for inter-process communication and OpenMP for multi-threading within each process. The program takes a directed graph as input and generates K shortest paths between randomly selected pairs of source and sink nodes.

## Dependencies

- [MPI (Message Passing Interface)](https://docs.open-mpi.org/en/v5.0.x/)
- [OpenMP (Open Multi-Processing)](https://www.openmp.org/resources/refguides/)

## Installation

1. Install MPI and OpenMP libraries on your system.
2. Clone or download the repository containing the code.
3. Compile the code using a C compiler with MPI and OpenMP support.

## Setup and Execution Environment on Replit

### Environment Configuration

This code was executed and tested using Replit. To set up the environment for running MPI (Message Passing Interface), the following steps were undertaken:

1. **Replit Environment**: We created a Replit workspace to facilitate code development and execution.

2. **Cluster Setup with Nix**: A file named `shell.nix` was created to configure the cluster environment. The contents of the `shell.nix` file are as follows:

    ```
    with import <nixpkgs> {};

    mkShell {
      buildInputs = [
        mpich
      ];
    }
    ```

3. **Terminal Commands**: After configuring the cluster environment, the following terminal commands were executed:

    ```
    nix-shell
    ```

### Compilation and Execution

Once the environment was set up, MPI commands were executed as follows:

1. **Compilation**:
   
   ```
   mpicc -o your_executable_name your_source_code.c -fopenmp 
   ```

2. **Thread Configuration (Optional)**:

   If the number of threads was not set within the code using  

   ```
   omp_set_num_threads(NUM_THREADS)
   ```

   or by other methods, then the following command was used to specify the number of threads:

   ```
   export OMP_NUM_THREADS=num_threads
   ```

3. **Execution**:

   After compilation and thread configuration (if necessary), the MPI code was run using the following 
   command:

   ```
   mpirun -np num_processes ./your_executable_name
   ```

### Dataset Files

The dataset files used for testing are as follows:

- `mapped-doctor-who.txt`
- `mapped-new-who.txt`
- `mapped-classic-who.txt`

These files were obtained by running `dataProcessing.c` using the following commands:

    ```
    gcc -o obj_name dataProcessing.c
    ./obj_name
    ```

There's no need to run dataProcessing.c file again because data is already processed and converted into numerical representation in above text files that can be stored in matrix graph.

## Usage

1. Ensure that the input graph is in the proper format. The program expects the graph data to be in a file named "mapped-new-who.txt" with the following format:

    ```
    from_node to_node weight
    ```
2. Execute the compiled program with the appropriate number of MPI processes and OpenMP threads.

3. To load different dataset in serial and parallel code, change the text file name and change the max number of nodes according to that dataset (both, in queue.h header file and the respective code file).
 

## Algorithm Overview

The algorithm follows these main steps:

1. Read the graph data from the input file and distribute it among MPI processes.
2. Randomly select source-sink node pairs and broadcast them to all processes.
3. Each process calculates the shortest path from its source node to the sink node using Dijkstra's algorithm.
4. Parallelize the process of finding K shortest paths by distributing work among MPI processes and using OpenMP threads within each process.
5. Merge the locally computed shortest paths from all processes to obtain the final K shortest paths.

## Performance

The performance of the algorithm is evaluated based on its execution time, which is influenced by factors such as the size of the graph, the number of MPI processes, and the number of OpenMP threads. Performance improvements can be achieved by optimizing the parallelization strategy and reducing communication overhead.
