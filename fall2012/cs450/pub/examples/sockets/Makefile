BINARIES=htons client server udpclient udpserver selectserver gethostbyname

all: $(BINARIES)

clean:
	rm $(BINARIES)

$*: $@.c
	gcc -g -o $@ $?

