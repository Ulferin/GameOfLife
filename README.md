# CLI-based Game of Life

The project is relative to a Parallel and Distributed System course assignment. It contains implementations of the [Conway's Game Of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) using different approaches with the aim of visualizing _speedup_ and _scalability_ of the parallel versions.

The implemented versions are:
- `gol-seq.cpp`: sequential implementation.
- `gol-par.cpp`: parallel implementation using only C++ threads from thread library.
- `gol-omp.cpp`: parallel implementation using OpenMP parallel for construct.

All the implemented versions use a _matrix-like_ data structure, where each entry of the matrix represents the status of a cell in the Conway's GOL.

The mandatory parameters to execute the tests are:
- nrows: total number of rows the board must contain;
- ncols: total number of columns the board must contain;
- iters: how many times the status of the board will be updated;
- seed: seed to be provided to the random number generator;
- wait time: used for debugging, specify how much time to wait before computing the next board update;
- nworkers: required only in parallel versions, specify the total number of workers.

A `Makefile` is provided and can be used to compile and test the code. For a complete description of implementation and results, refer to [the project report](GOLReport.pdf).
