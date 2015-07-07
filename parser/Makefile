CC=icpc
CPPFGLAGS=-g -O3

TARGET=csr2bin

all:$(TARGET)

csr2bin: csr2bin.cpp
	$(CC) $(CPPFLAGS) $^ -o $@

clean:
	rm -rf $(TARGET) *.o
