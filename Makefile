TARGET=csa
CC=g++
CFLAGS=-g -Wall

all: $(TARGET)
	sudo iptables -F



$(TARGET): main.cpp
	$(CC) $(CFLAGS) $^ -o $@ -lpcap

clean:
	rm -f $(TARGET) *.o
	sudo iptables -F