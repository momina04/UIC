OBJECTS=bencode-tools-2011-03-15/bencode.o
INCLUDES=bencode-tools-2011-03-15/include/ 

all: 
	#echo "Don't forget to install OpenSSL and libcurl3-dev"
	gcc -g --std=gnu99 -I $(INCLUDES) hw4.c hw4_bencode.c $(OBJECTS) -o hw4 -lssl -lcurl 2>foo
#    if [ $($?) = 0 ];
#    then
#    @echo "[COMPILE SUCCESS]"
#    else
#    vi -q foo
#    fi


clean:
	rm hw4
	rm foo

run: clean all
	./hw4 cs450files.torrent
	cd cs450files
	md5sum * > ~/a
	echo "---------------------Diff------------------------"
	vimdiff ~/a ~/b

select:
	rm sc
	rm ss
	gcc -g select_server.c -o ss 2>foo_ss
	gcc -g select_client.c -o sc 2>foo_sc

