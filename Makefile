USE_MIC=NO
DEBUG=YES

CC=icpc
FLAGS=-restrict -O3
LIBS=
INCS=
DEFINES=

ifeq ($(USE_MIC),YES)
	FLAGS+=-mmic
endif

ifeq ($(DEBUG),YES)
	FLAGS+=-g
	DEFINES+=-DTESTING
endif

TARGET=spmv

all:$(TARGET)

spmv: spmv.o interfaces.o
	$(CC) $(DEFINES) $(FLAGS) $+ -o $@ $(INCS) $(LIBS)

spmv.o: ./src/spmv.cpp
	$(CC) $(DEFINES) $(FLAGS) $+ -c $(INCS) $(LIBS)

interfaces.o: ./inc/interfaces.h ./src/interfaces.cpp
	$(CC) $(DEFINES) $(FLAGS) $+ -c $(INCS) $(LIBS)

clean:
	rm -rf obj/* bin/* *.o $(TARGET)

run:
	./bin/spmv
