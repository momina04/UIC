
run: clean upload compile submit tail

clean:
	 ssh ragarw8@argo.cc.uic.edu touch  mpi_stdout; touch mpi_stderr; rm mpi_std*; touch mpi_stdout; touch mpi_stderr; touch mpi.o; rm mpi.o

upload:
	scp ring_minimum1.c ragarw8@argo.cc.uic.edu:~
	scp script.sh ragarw8@argo.cc.uic.edu:~

compile:
	ssh ragarw8@argo.cc.uic.edu /usr/common/mpich2/bin/mpicc ring_minimum1.c  -o mpi.o -std=gnu99 >foo

submit:
	ssh ragarw8@argo.cc.uic.edu qsub script.sh
	@echo "[JOB SUBMITTED]"

tail:
	ssh ragarw8@argo.cc.uic.edu tail -f mpi_std*

cat:
	ssh ragarw8@argo.cc.uic.edu cat mpi_std*
	@echo "[CAT'ED]"

list:
	ssh ragarw8@argo.cc.uic.edu qselect -u ragarw8
	#qselect -u ragarw8
	@echo "[JOBS LISTED]"

delete:
	ssh ragarw8@argo.cc.uic.edu qselect -u ragarw8|xargs qdel
	#qselect -u ragarw8|xargs qdel
	@echo "[JOBS DELETED]"


