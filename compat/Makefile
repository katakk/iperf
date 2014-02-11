OUT=libcompat.a
OBJS=Thread.o error.o delay.o gettimeofday.o inet_ntop.o inet_pton.o signal.o snprintf.o string.o
CC=gcc
CXX=g++
INCLUDES=-I. -I. -I..  -I../include -I../include  
CFLAGS=-Wall -DHAVE_CONFIG_H -O2 
AR=ar
ARFLAGS = cru
RANLIB=ranlib

all: $(OUT)
clean:
	-rm -f $(OUT)
	-rm -f $(OBJS)

.c.o:
	$(CC) $(INCLUDES) $(CFLAGS) -c -o $@ $<;

.cpp.o:
	$(CXX) $(INCLUDES) $(CFLAGS) -c -o $@ $<;

$(OUT): $(OBJS)
	-rm -f $(OUT)
	$(AR) $(ARFLAGS) $(OUT) $(OBJS)
	$(RANLIB) $(OUT)


