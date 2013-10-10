#!/bin/bash
#PBS -l walltime=5:00
#PBS -l cput=5:00
#PBS -l nodes=4:ppn=1
#PBS -o mpi_stdout
#PBS -e mpi_stderr
cd /home/homes50/ragarw8
/usr/common/mpich2/bin/mpiexec mpi.o

