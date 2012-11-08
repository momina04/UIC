TARGETS=hw6_sender hw6_receiver proxy

all: $(TARGETS)

hw6_sender: hw6_sender.c hw6.c
	gcc -g --std=gnu99 -o hw6_sender hw6_sender.c hw6.c

hw6_receiver: hw6_receiver.c hw6.c
	gcc -g --std=gnu99 -o hw6_receiver hw6_receiver.c hw6.c

proxy: proxy.c
	gcc --std=gnu99 -g -o proxy proxy.c

clean:
	rm -f $(TARGETS)

