TARGETS=hw5_sender hw5_receiver proxy

all: $(TARGETS)

hw5_sender: hw5_sender.c hw5.c
	gcc --std=gnu99 -o hw5_sender hw5_sender.c hw5.c -g 2>foo

hw5_receiver: hw5_receiver.c hw5.c
	gcc --std=gnu99 -o hw5_receiver hw5_receiver.c hw5.c -g 2>foo2

proxy: proxy.c
	gcc --std=gnu99 -g -o proxy proxy.c

clean:
	rm -f $(TARGETS)

run: all
	#pkill -9 hw5_receiver
	#pkill -9 hw5_sender
	./hw5_receiver 127.0.0.1 8000 > rx &
	#./proxy -p 8000 -b 2 -l 10 -f 1 &
	./proxy -p 8000 -b 200 -l 1 -f 1&
	cat 1000lines | ./hw5_sender 127.0.0.1 8000 2> sender_output
	pkill -9 proxy
	#kill `pgrep -f "./proxy -p 8000"`
	cat sender_output | grep -i "KB/s" | awk '{print $(NF-1)}'
	echo "************** DIFF *******************"
	cat rx
	#diff 1000lines rx
