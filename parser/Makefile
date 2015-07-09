CC=icpc
CPPFGLAGS=-g -O3

TARGET=csr2bin ell2bin ell1_2bin

all:$(TARGET)

csr2bin: csr2bin.cpp
	$(CC) $(CPPFLAGS) $^ -o $@

ell2bin: ell2bin.cpp
	$(CC) $(CPPFLAGS) $^ -o $@

ell1_2bin: ell1_2bin.cpp
	$(CC) $(CPPFLAGS) $^ -o $@


clean:
	rm -rf $(TARGET) *.o
