CC=g++
CFLAGS=-Wall -g
CERROR=foo

string: string.cpp
	$(CC) $(CFLAGS) -c string.cpp -o string.o 2>$(CERROR)

painting: painting.cpp
	$(CC) $(CFLAGS) -c painting.cpp -o painting.o 2>$(CERROR)

linked_list: linked_list.cpp
	$(CC) $(CFLAGS) -c linked_list.cpp -o linked_list.o 2>$(CERROR)

artist: artist.cpp
	$(CC) $(CFLAGS) -c artist.cpp -o artist.o 2>$(CERROR)

hw1: hw1.cpp painting linked_list artist
	@echo "Compiler       : $(CC)"
	@echo "Compiler Flags : $(CFLAGS)"
	@echo "Error File     : $(CERROR)"
	@echo "Compiling hw1.cpp"
	$(CC) $(CFLAGS) hw1.cpp painting.o linked_list.o artist.o -o hw1 2>$(CERROR)

all: hw1

run: clean all
	./hw1 <test_input >output

clean:
	@echo Cleaning
	rm -f painting.o linked_list.o artist.o hw1
	rm -f foo
	@echo Clean Complete.

