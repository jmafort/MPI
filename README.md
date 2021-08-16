# MPI

## Implementation of the Leibniz Series

The code uses the Leibniz Series to calculate the PI approximation, which you can find [here](https://en.wikipedia.org/wiki/Leibniz_formula_for_%CF%80).

I've also used the MPI lib and the `C++` language.

## Environment Setup

If you're on a Windows machine you can experiment with this code by following this steps: [MPI Setup w/ Visual Studio](https://www.youtube.com/watch?v=Ol5L0IHXiOE).
Then you can clone this repository or download the `Origem.cpp` file and replace the `Source.cpp` shown in the video.

If you're on a Linux machine you can follow this steps: [MPI Setup on Linux](https://www.youtube.com/watch?v=QIMAu_o_5V8).

## Commands

To run the executable created with the Visual Studio you can run this command:

```bash
    mpiexec <-n numberofprocesses> ./MPI
```
(If the `-n` command is not present you don't need to set the number of processes. The program will run with one process.)

In the Linux environment you need to compile it first with:

```bash
    mpic++ my_mpi_application.cpp -o my_mpi_application
```

## Parallel Programming

This code takes as input an int that represents the number of parts of the sum.
It then divides the series into equal amounts to distribute among all processes. 
After that, the code takes all the partial sums and adds them togheter to get an approximation of the PI/4.

### Note: the number of parts of the sum must be evenly divided by the number of processes, otherwise you'll get an error and the program will be shut down.
