#No. of physical processors
P=2
#No. of logical processes per processor
K=2
#No. of inputs
N=16



upload: 
	@scp hw3.c ragarw8@argo.cc.uic.edu:~
	@scp script.sh ragarw8@argo.cc.uic.edu:~
	@scp input ragarw8@argo.cc.uic.edu:~

compile:  upload
	@ssh ragarw8@argo.cc.uic.edu /usr/common/mpich2/bin/mpicc hw3.c  -o mpi.o -std=gnu99 -lm 2>foo

#submit: input_gen upload compile
submit:
	@scp input ragarw8@argo.cc.uic.edu:~
	@echo "N=$(N), P=$(P), K=$(K)"
	@echo "------------------------"
	@cat input
	@echo "------------------------"
	@ssh ragarw8@argo.cc.uic.edu "qsub -l nodes=$(P):ppn=$(K) script.sh"
	@echo "[JOB SUBMITTED]"

tail:
	@ssh ragarw8@argo.cc.uic.edu tail -f mpi_std*
	#ssh argo.cc.uic.edu 'while [ ! -s mpi_stdout ];   do   printf "" ; done; cat mpi_stdout' >> stdout_all
	#ssh argo.cc.uic.edu 'echo $(N) $(K) $(P) `cat mpi_stdout|tail -1|cut -f 8 -d " "`' >> result_file_hypercube
cat:
	@ssh ragarw8@argo.cc.uic.edu cat mpi_std*
	@echo "[CAT'ED]"

list:
	@ssh ragarw8@argo.cc.uic.edu qselect -u ragarw8
	@echo "[JOBS LISTED]"

delete:
	@ssh ragarw8@argo.cc.uic.edu "qselect -u ragarw8|xargs qdel"
	@echo "[JOBS DELETED]"

input_gen:
	@touch input
	@rm input
	@gcc gen_input.c -o gen_input
	@gen_input $(N) > input

