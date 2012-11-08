CC=gcc
CFLAGS=-Wall -g
CERROR=foo

hw1: hw1.c
	@echo "Compiler       : $(CC)"
	@echo "Compiler Flags : $(CFLAGS)"
	@echo "Error File     : $(CERROR)"
	@echo "Compiling hw1.c"
	$(CC) $(CFLAGS) hw1.c -o hw1 2>$(CERROR)

all: hw1

run: clean all
	./hw1

clean:
	@echo Cleaning
	rm -f hw1
	rm -f foo
