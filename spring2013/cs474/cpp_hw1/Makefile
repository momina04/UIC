CC=g++
CFLAGS=-Wall -g
CERROR=foo

string: string.cpp string.h
	$(CC) $(CFLAGS) -c string.cpp -o string.o 2>$(CERROR)

test_string: test_string.cpp string
	$(CC) $(CFLAGS)  test_string.cpp string.o -o test_string.o 2>$(CERROR)
	echo "test_string_input"|valgrind --tool=memcheck ./test_string.o >valgrind_test_string_stdout 2>valgrind_test_string_stderr
	vi -p valgrind_test_string*

linked_list: linked_list.cpp linked_list.h
	$(CC) $(CFLAGS) -c linked_list.cpp -o linked_list.o 2>$(CERROR)

test_linked_list: test_linked_list.cpp linked_list string painting artist
	$(CC) $(CFLAGS)  test_linked_list.cpp string.o linked_list.o painting.o artist.o -o test_linked_list.o 2>$(CERROR)
	valgrind --tool=memcheck ./test_linked_list.o >valgrind_test_linked_list_stdout 2>valgrind_test_linked_list_stderr
	vi -p valgrind_test_linked_list*

painting: painting.cpp painting.h artist
	$(CC) $(CFLAGS) -c painting.cpp -o painting.o 2>$(CERROR)

artist: artist.cpp artist.h painting
	$(CC) $(CFLAGS) -c artist.cpp -o artist.o 2>$(CERROR)

painting_manager: painting_manager.cpp painting_manager.h string linked_list painting artist
	$(CC) $(CFLAGS) -c painting_manager.cpp string.o linked_list.o painting.o artist.o -o painting_manager.o 2>$(CERROR)

hw1: hw1.cpp painting linked_list artist
	@echo "Compiler       : $(CC)"
	@echo "Compiler Flags : $(CFLAGS)"
	@echo "Error File     : $(CERROR)"
	@echo "Compiling hw1.cpp"
	$(CC) $(CFLAGS) hw1.cpp painting.o linked_list.o artist.o -o hw1 2>$(CERROR)

all: hw1

run: clean all
	./hw1 <test_input >output

test_clean:
	rm test_string.o


clean: test_clean
	@echo Cleaning
	rm -f painting.o linked_list.o artist.o hw1
	rm -f foo
	@echo Clean Complete.

