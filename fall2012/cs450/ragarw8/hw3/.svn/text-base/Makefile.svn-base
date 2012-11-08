CC=gcc
CFLAGS=-Wall -g 
CERROR=foo

hw3: hw3.c
	@echo "Compiler       : $(CC)"
	@echo "Compiler Flags : $(CFLAGS)"
	@echo "Error File     : $(CERROR)"
	@echo "Compiling hw3.c"
	$(CC) $(CFLAGS) hw3.c -o hw3 2>$(CERROR)
#	if [ $$? = 0 ];
#	then
#		@echo "[COMPILE SUCCESS]"
#	else
#		vi -q foo
#	fi
	

all: hw3

run: clean all
#	./hw3_score-modified ritesh 8086 score

clean:
	@echo Cleaning
	rm -f hw3
	rm -f foo
