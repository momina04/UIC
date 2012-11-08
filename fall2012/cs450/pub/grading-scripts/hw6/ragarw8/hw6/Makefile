TARGETS=hw6_sender hw6_receiver proxy

all: $(TARGETS)

hw6_sender: hw6_sender.c hw6.c
	gcc -g --std=gnu99 -o hw6_sender hw6_sender.c hw6.c 2>foo

hw6_receiver: hw6_receiver.c hw6.c
	gcc -g --std=gnu99 -o hw6_receiver hw6_receiver.c hw6.c 2>foo

proxy: proxy.c
	gcc --std=gnu99 -g -o proxy proxy.c 2>foo

clean:
	rm -f $(TARGETS)

