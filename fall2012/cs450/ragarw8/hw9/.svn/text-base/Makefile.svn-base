#------------------------------------------------------------------------------
# File: Makefile
#
# Note: This Makefile requires GNU make.
#
# (c) 2001,2000 Stanford University
#
#------------------------------------------------------------------------------



all : sr

CC = gcc

OSTYPE = $(shell uname)

ifeq ($(OSTYPE),CYGWIN_NT-5.1)
ARCH = -D_CYGWIN_
endif

ifeq ($(OSTYPE),Linux)
ARCH = -D_LINUX_
SOCK = -lnsl -lresolv
endif

ifeq ($(OSTYPE),SunOS)
ARCH =  -D_SOLARIS_
SOCK = -lnsl -lsocket -lresolv
endif

ifeq ($(OSTYPE),Darwin)
ARCH = -D_DARWIN_
SOCK = -lresolv
endif

CFLAGS = -g -o0 -Wall -std=gnu99 -D_DEBUG_ $(ARCH)

LIBS= $(SOCK) -lm -lpthread
PFLAGS= -follow-child-processes=yes -cache-dir=/tmp/${USER}
PURIFY= purify ${PFLAGS}

sr_SRCS = sr_router.c sr_main.c  \
          sr_if.c sr_rt.c sr_vns_comm.c   \
          sr_dumper.c sha1.c rmutex.c

sr_OBJS = $(patsubst %.c,%.o,$(sr_SRCS))
sr_DEPS = $(patsubst %.c,.%.d,$(sr_SRCS))

$(sr_OBJS) : %.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@ 2>foo

$(sr_DEPS) : .%.d : %.c
	$(CC) -MM $(CFLAGS) $<  > $@

include $(sr_DEPS)	

sr : $(sr_OBJS)
	$(CC) $(CFLAGS) -o sr $(sr_OBJS) $(LIBS)

sr.purify : $(sr_OBJS)
	$(PURIFY) $(CC) $(CFLAGS) -o sr.purify $(sr_OBJS) $(LIBS)

.PHONY : clean clean-deps dist

clean:
	rm -f *.o *~ core sr *.dump *.tar tags

clean-deps:
	rm -f .*.d

dist-clean: clean clean-deps
	rm -f .*.swp sr_stub.tar.gz

dist: dist-clean
	(cd ..; tar -X stub/exclude -cvf sr_stub.tar stub/; gzip sr_stub.tar); \
    mv ../sr_stub.tar.gz .

submit: dist-clean
	rm -f project2.tgz; tar czvf project2.tgz *; turnin project2.tgz -c cs123f -p project2

tags:
	ctags *.c *.h

run: all
	touch dump.tcpdump
	touch dump.bak
	cp dump.bak dump.bak2
	cp dump.tcpdump dump.bak
	#./sr -s vns-1.stanford.edu -t 1245 -u ragarw8 -l dump.tcpdump
	gdb -x gdbinit --args ./sr -s vns-1.stanford.edu -t 1245 -u ragarw8 -l dump.tcpdump

dump:
	tcpdump  -r dump.tcpdump -n 
	#tcpdump  -r dump.tcpdump -vvvv -n -xx

dump2:
	tcpdump  -r dump.bak -vvvv -n -xx

ping:
	ping -c 1 171.67.237.237 #server1
	#ping -c 1 171.67.237.239 #server2

pingrouter:
	ping -c 1 171.67.237.232 #router

tracerouter:
	traceroute -q 1 -N 1 171.67.237.237 #server1
	#traceroute 171.67.237.239 #server2

tracerouterouter:
	traceroute -q 1 -N 1 171.67.237.232 #router

test:
	@touch test_log
	@rm test_log
	@echo "Pinging Application Servers" >> test_log
	@echo "{" >> test_log
	@echo "{" >> test_log
	@ping -c 5 171.67.237.237 >> test_log
	@echo "}" >> test_log
	@echo "{" >> test_log
	@ping -c 5 171.67.237.239 >> test_log
	@echo "}" >> test_log
	@echo "}" >> test_log
	@echo "Pinging Router Interfaces" >> test_log
	@echo "{" >> test_log
	@echo "{" >> test_log
	@ping -c 5 171.67.237.232 >> test_log
	@echo "}" >> test_log
	@echo "{" >> test_log
	@ping -c 5 171.67.237.236 >> test_log
	@echo "}" >> test_log
	@echo "{" >> test_log
	@ping -c 5 171.67.237.238>> test_log
	@echo "}" >> test_log
	@echo "}" >> test_log
	@echo "Tracerouting Application Servers" >> test_log
	@echo "{" >> test_log
	@echo "{" >> test_log
	@traceroute 171.67.237.237 >> test_log
	@echo "}" >> test_log
	@echo "{" >> test_log
	@traceroute 171.67.237.239 >> test_log
	@echo "}" >> test_log
	@echo "}" >> test_log
	@echo "Tracerouting Router Interfaces" >> test_log
	@echo "{" >> test_log
	@echo "{" >> test_log
	@traceroute 171.67.237.232 >> test_log
	@echo "}" >> test_log
	@echo "{" >> test_log
	@traceroute 171.67.237.236 >> test_log
	@echo "}" >> test_log
	@echo "{" >> test_log
	@traceroute 171.67.237.238>> test_log
	@echo "}" >> test_log
	@echo "}" >> test_log
	@echo "Test Complete...see test_log"
