
all: compile 

compile:
	mpicc ring_minimum1.c  -o mpi.o -std=gnu99 2>foo
#    ifeq ($?,0)
#		@echo "[COMPILE SUCCESS]"
#    else
#		vi -q foo
#    endif

run: clean compile submit tail
submit:
	scp mpi.o ragarw8@argo.cc.uic.edu:~
	scp script.sh ragarw8@argo.cc.uic.edu:~
	ssh ragarw8@argo.cc.uic.edu qsub script.sh
	@echo "[JOB SUBMITTED]"

cat:
	ssh ragarw8@argo.cc.uic.edu cat mpi_std*

tail:
	ssh ragarw8@argo.cc.uic.edu tail -f mpi_std*

list:
	ssh ragarw8@argo.cc.uic.edu qselect -u ragarw8
	@echo "[JOBS LISTED]"

delete:
	ssh ragarw8@argo.cc.uic.edu qselect -u ragarw8|xargs qdel
	@echo "[JOBS DELETED]"

clean:
	touch mpi.o
	rm mpi.o
	ssh ragarw8@argo.cc.uic.edu "touch mpi_stdout; touch mpi_stderr; rm mpi_std*; touch mpi_stdout; touch mpi_stderr; touch mpi.o; rm mpi.o"
