CC=gcc
CFLAGS=-Wall -g -pthread
CERROR=foo

hw2: hw2.c
	@echo "Compiler       : $(CC)"
	@echo "Compiler Flags : $(CFLAGS)"
	@echo "Error File     : $(CERROR)"
	@echo "Compiling hw2.c"
	$(CC) $(CFLAGS) hw2.c -o hw2 2>$(CERROR)
#	if [ $$? = 0 ];
#	then
#		@echo "[COMPILE SUCCESS]"
#	else
#		vi -q foo
#	fi
	

all: hw2

run: clean all
	./hw2_score-modified ritesh 8086 score

clean:
	@echo Cleaning
	rm -f hw2
	rm -f foo
