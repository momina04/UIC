#No. of physical processors
P=2
#No. of logical processes per processor
K=2
#No. of inputs
N=8

run: clean input upload compile submit tail

clean:
	 ssh ragarw8@argo.cc.uic.edu "touch mpi_stdout; touch mpi_stderr; rm mpi_std*; touch mpi_stdout; touch mpi_stderr; touch mpi.o; rm mpi.o; touch input; rm input"

upload: ring_minimum1.c script.sh input_gen
	scp *.c ragarw8@argo.cc.uic.edu:~
	#scp ring_minimum1.c ragarw8@argo.cc.uic.edu:~
	scp script.sh ragarw8@argo.cc.uic.edu:~
	scp input ragarw8@argo.cc.uic.edu:~

compile: ring_minimum1.c hw1-ring.c
	#ssh ragarw8@argo.cc.uic.edu /usr/common/mpich2/bin/mpicc ring_minimum1.c  -o mpi.o -std=gnu99 2>foo
	ssh ragarw8@argo.cc.uic.edu /usr/common/mpich2/bin/mpicc hw1-ring.c  -o mpi.o -std=gnu99 2>foo

submit: input_gen
	@echo "N=$(N), P=$(P), K=$(K)"
	ssh ragarw8@argo.cc.uic.edu "qsub -l nodes=$(P):ppn=$(K) script.sh"
	@echo "[JOB SUBMITTED]"

tail:
	ssh ragarw8@argo.cc.uic.edu tail -f mpi_std*

cat:
	ssh ragarw8@argo.cc.uic.edu cat mpi_std*
	@echo "[CAT'ED]"

list:
	ssh ragarw8@argo.cc.uic.edu qselect -u ragarw8
	@echo "[JOBS LISTED]"

delete:
	ssh ragarw8@argo.cc.uic.edu "qselect -u ragarw8|xargs qdel"
	@echo "[JOBS DELETED]"

input_gen:
	rm input
	gcc gen_input.c -o gen_input
	gen_input $(N) > input
