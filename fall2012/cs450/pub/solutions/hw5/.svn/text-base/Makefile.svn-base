TARGETS=hw5_sender hw5_receiver proxy

all: $(TARGETS)

hw5_sender: hw5_sender.c hw5.c
	gcc -g --std=gnu99 -o hw5_sender hw5_sender.c hw5.c

hw5_receiver: hw5_receiver.c hw5.c
	gcc -g --std=gnu99 -o hw5_receiver hw5_receiver.c hw5.c

proxy: proxy.c
	gcc --std=gnu99 -g -o proxy proxy.c

clean:
	rm -f $(TARGETS)

