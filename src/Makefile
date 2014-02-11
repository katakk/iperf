EXE=iperf

OBJS=Client.o Extractor.o Launch.o List.o Listener.o Locale.o PerfSocket.o ReportCSV.o \
     ReportDefault.o Reporter.o Server.o Settings.o SocketAddr.o \
     gnu_getopt.o gnu_getopt_long.o main.o service.o sockets.o \
     stdio.o tcp_window_size.o \
     ../compat/libcompat.a

CC=gcc
CXX=g++
INCLUDES=-I. -I. -I..  -I../include -I../include
CFLAGS=-Wall -DHAVE_CONFIG_H -O2 -pthread
AR=ar
ARFLAGS = cru
RANLIB=ranlib

all: $(EXE)

clean:
	-rm -f $(EXE)
	-rm -f $(OBJS)

.c.o:
	$(CC) $(INCLUDES) $(CFLAGS) -c -o $@ $<;

.cpp.o:
	$(CXX) $(INCLUDES) $(CFLAGS) -c -o $@ $<;

$(EXE): $(OBJS)
	$(CXX) $(CFLAGS) -o $(EXE) $(OBJS)

