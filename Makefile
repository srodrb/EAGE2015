USE_MIC=NO


CC=icpc
FLAGS=-restrict -O3 -g
LIBS=
INCS=

ifeq ($(USE_MIC),YES)
	FLAGS+=-mmic
endif

TARGET=spmv

all:$(TARGET)

spmv: spmv.o interfaces.o
	$(CC) $(FLAGS) $+ -o $@ $(INCS) $(LIBS)

spmv.o: ./src/spmv.cpp
	$(CC) $(FLAGS) $+ -c $(INCS) $(LIBS)

interfaces.o: ./inc/interfaces.h ./src/interfaces.cpp
	$(CC) $(FLAGS) $+ -c $(INCS) $(LIBS)

clean:
	rm -rf obj/* bin/* *.o $(TARGET)

run:
	./bin/spmv
