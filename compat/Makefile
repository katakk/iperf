.SUFFIXES: .cpp .c .o
include ../make.env

OUT=libcompat.a

OBJS=Thread.o error.o delay.o gettimeofday.o inet_ntop.o inet_pton.o signal.o snprintf.o string.o

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

