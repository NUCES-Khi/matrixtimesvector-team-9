#!/bin/bash

# Compilation
gcc -o sequential_Matrix sequential_Matrix.c -lm
gcc -o omp_naive omp_naive.c -fopenmp -lm
mpicc -o mpi_naive mpi_naive.c -lm

# Input sizes
sizes=(64 128 256 512 1024 2048 4096 8192 16384 32768)

# Number of repetitions for averaging
repetitions=10

# Initialize counters
count=1
average_time=0

# Header
echo "test S.no, file, input size, time taken, average so far"

# Benchmark each program for each input size
for size in "${sizes[@]}"; do
    for program in "sequential_Matrix" "omp_naive" "mpi_naive"; do
        for ((i=1; i<=repetitions; i++)); do
            start_time=$(date +%s.%N)
            ./$program $size $size >/dev/null 2>&1
            end_time=$(date +%s.%N)
            elapsed_time=$(bc <<< "scale=6; $end_time - $start_time")
            average_time=$(bc <<< "scale=6; ($average_time * ($count - 1) + $elapsed_time) / $count")
            echo "$count, $program, $size, $elapsed_time secs, $average_time secs"
            ((count++))
        done
    done
done

