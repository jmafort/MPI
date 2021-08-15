//Grupo: Jovane Mafort e Joelson Ferreira

//Aproximacao de Pi usando a Serie de Leibniz

#define _USE_MATH_DEFINES

#include <cmath>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <iostream>

using namespace std;

void Get_Dimension(int& size, int& local_size, int my_rank, int comm_size, MPI_Comm comm) {
    //Gets the size of the vector if I'm the root process
    if (my_rank == 0) {
        cout << "Enter the size of the vector: " << endl;
        cin >> size;
    }
    //The root process sends the size of the vector to all other processes
    MPI_Bcast(&size, 1, MPI_INT, 0, comm);

    //Verifies if the vector can be evenly divided among the processes
    if ((size <= 0) || (size % comm_size != 0)) {
        printf("Error: The size should be positive and evenly divisible by the number of processes!\n");
        MPI_Finalize();
        exit(-1);
    }
    //Divides the vector evenly
    local_size = size / comm_size;
}

void Get_Array_Values(vector<int>& local_x, int vector_size, int local_vector_size, int my_rank, MPI_Comm comm) {
    int i;
    //An auxiliary vector to fill and scatter
    vector<int> aux;

    //If I'm the root process, fill the vector
    if (my_rank == 0) {
        for (i = 0; i < vector_size; i++) {
            //Fill only with odd numbers to use in the Leibniz Series
            aux.push_back(i*2+1);
        }
    }
    //The root distributes the vector's data evenly among all the processes
    MPI_Scatter(static_cast<void*>(aux.data()), local_vector_size, MPI_INT, static_cast<void*>(local_x.data()), local_vector_size, MPI_INT, 0, comm);
}

void Print_Array(const char title[], vector<int> local_array, int vector_size, int local_vector_size, int my_rank, MPI_Comm comm) {
    //An auxiliary vector to gather all the values from local arrays and print them
    vector<int> aux(vector_size);
    int i;

    //If I'm the root process I'll gather all the values and print them
    if (my_rank == 0) {
        MPI_Gather(static_cast<void*>(local_array.data()), local_vector_size, MPI_INT, static_cast<void*>(aux.data()), local_vector_size, MPI_INT, 0, comm);
        printf("\nThe Vector %s\n", title);
        for (int i = 0; i < vector_size; i++) {
            printf("%5d ", aux[i]);
        }
        printf("\n");
    }
    //If I'm any other process I'll just send the values to the root
    else {
        MPI_Gather(static_cast<void*>(local_array.data()), local_vector_size, MPI_INT, static_cast<void*>(aux.data()), local_vector_size, MPI_INT, 0, comm);
    }
}

void Pi_Aproximation(vector<int>& local_x, double &pi, int vector_size, int local_vector_size, MPI_Comm comm) {
    int local_i;
    double local_pi = 0;

    //Calculates the local aproximation of pi using the Leibniz Series
    for (local_i = 0; local_i < local_vector_size; local_i++) {
        if (local_i % 2 == 0) {
            local_pi = local_pi + 1 / (double)local_x[local_i];
        }
        else {
            local_pi = local_pi - 1 / (double)local_x[local_i];
        }
    }

    //Makes the partial sum of the local approximations of pi and sends to the root
    MPI_Reduce(&local_pi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, comm);
}

int main(int argc, char** argv) {
    //Size of the entire vector and size of each process' vector
    int vector_size, local_vector_size;
   
    int comm_size, my_rank;
    MPI_Comm comm;

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    comm = MPI_COMM_WORLD;

    // Get the number of processes
    MPI_Comm_size(comm, &comm_size);

    // Get the rank of the process
    MPI_Comm_rank(comm, &my_rank);

    //Get the greatest elapsed time among all processes
    double local_start_time, local_finish_time, local_time_elapsed, time_elapsed;
    
    //Get dimension for the local vectors given a number of processes
    Get_Dimension(vector_size, local_vector_size, my_rank, comm_size, comm);
    vector<int> local_x(local_vector_size);
    double pi;

    //Barrier to count the elapsed time
    MPI_Barrier(comm);
    local_start_time = MPI_Wtime();
    //---------------------------------Program Logic-------------------------------------//
    Get_Array_Values(local_x, vector_size, local_vector_size, my_rank, comm);
    Print_Array("x", local_x, vector_size, local_vector_size, my_rank, comm);

    Pi_Aproximation(local_x, pi, vector_size, local_vector_size, comm);

    //-----------------------------------------------------------------------------------//
    local_finish_time = MPI_Wtime();
    local_time_elapsed = local_finish_time - local_start_time;
    //Sends to the root process the greatest time elapsed among all processes
    MPI_Reduce(&local_time_elapsed, &time_elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, comm);

    //Root process prints all information about the Pi aproximation and the time elapsed
    if (my_rank == 0) {
        printf("\nPi aproximation: %f\n", 4 * pi);
        printf("\nPi aproximation in cmath: %f\n", M_PI);
        printf("\nPi aproximation error: %f\n", M_PI - 4 * pi);
        printf("\nElapsed time: %.4f seconds.\n", time_elapsed);
    }

    // Finalize the MPI environment
    MPI_Finalize();

    return 0;
}