.SUFFIXES: .cpp .c .o
include ../make.env

EXE=iperf

STATIC=-static

OBJS=Client.o Extractor.o Launch.o List.o Listener.o Locale.o PerfSocket.o ReportCSV.o \
     ReportDefault.o Reporter.o Server.o Settings.o SocketAddr.o \
     obsd_strlcpy.o obsd_strlcat.o \
     obsd_getopt_long.o obsd_getsubopt.o \
     main.o service.o sockets.o \
     stdio.o tcp_window_size.o \
     Thread.o error.o delay.o gettimeofday.o inet_ntop.o inet_pton.o signal.o snprintf.o string.o

all: $(EXE)

clean:
	-rm -f $(EXE)
	-rm -f $(OBJS)

.c.o:
	$(CC) $(INCLUDES) $(CFLAGS) -c -o $@ $<

.cpp.o:
	$(CXX) $(INCLUDES) $(CFLAGS) -c -o $@ $<

$(EXE): $(OBJS)
	$(CXX) $(CFLAGS) $(STATIC) -o $(EXE) $(OBJS)

