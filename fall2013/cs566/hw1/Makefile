#No. of physic:wal processors
P=2
#No. of logical processes per processor
K=2
#No. of inputs
N=8

run: clean upload compile submit tail

clean:
	 @ssh ragarw8@argo.cc.uic.edu "touch mpi_stdout; touch mpi_stderr; rm mpi_std*; touch Mpi_stdout; touch Mpi_stderr; touch mpi.o; rm mpi.o; touch input; rm input"

upload: ring_minimum1.c script.sh input_gen
	@scp *.c ragarw8@argo.cc.uic.edu:~
	@scp script.sh ragarw8@argo.cc.uic.edu:~
	@scp input ragarw8@argo.cc.uic.edu:~

compile: ring_minimum1.c hw1-ring.c
	#ssh ragarw8@argo.cc.uic.edu /usr/common/mpich2/bin/mpicc ring_minimum1.c  -o mpi.o -std=gnu99 -lm 2>foo
	#@ssh ragarw8@argo.cc.uic.edu /usr/common/mpich2/bin/mpicc hw1-ring.c  -o mpi.o -std=gnu99 -lm 2>foo
	@ssh ragarw8@argo.cc.uic.edu /usr/common/mpich2/bin/mpicc hw1-hypercube-method2.c  -o mpi.o -std=gnu99 -lm 2>foo

submit: input_gen
	@scp input ragarw8@argo.cc.uic.edu:~
	@echo "N=$(N), P=$(P), K=$(K)"
	@echo "------------------------"
	@cat input
	@echo "------------------------"
	@ssh ragarw8@argo.cc.uic.edu "qsub -l nodes=$(P):ppn=$(K) script.sh"
	@echo "[JOB SUBMITTED]"

tail:
	#@ssh ragarw8@argo.cc.uic.edu tail --retry mpi_std*
	ssh argo.cc.uic.edu 'while [ ! -s mpi_stdout ];   do   printf "" ; done'
	ssh argo.cc.uic.edu 'echo $(N) $(K) $(P) `cat mpi_stdout|tail -1|cut -f 8 -d " "`' >> result_file_hypercube
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
	@rm input
	@gcc gen_input.c -o gen_input
	@gen_input $(N) > input


#Below code is for generating final results for report
#All things to be run from argo
all2: compile2 gather_numbers

gather_numbers:  
	chmod a+x try.sh
	./try.sh

compile2:
	@ssh ragarw8@argo.cc.uic.edu "gcc gen_input.c -o gen_input; /usr/common/mpich2/bin/mpicc hw1-ring-method1.c -o hw1-ring-method1.o -std=gnu99 -lm;  /usr/common/mpich2/bin/mpicc hw1-ring-method2.c -o hw1-ring-method2.o -std=gnu99 -lm;  /usr/common/mpich2/bin/mpicc hw1-hypercube-method1.c -o hw1-ypercube-method1.o -std=gnu99 -lm;  /usr/common/mpich2/bin/mpicc hw1-hypercube-method2.c -o hw1-hypercube-method2.o -std=gnu99 -lm"


#Run this from client
upload2:
	@scp *.c ragarw8@argo.cc.uic.edu:~
	@scp Makefile ragarw8@argo.cc.uic.edu:~
	@scp *.sh ragarw8@argo.cc.uic.edu:~
