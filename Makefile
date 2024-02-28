CC = gcc
CFLAGS = -Wall -std=c2x
TARGET = practica_01
SRC = $(wildcard *.c) $(wildcard tad/*.c)
INCLUDE = ./
DST = bin

all: $(TARGET)

$(TARGET): $(SRC)
	@ mkdir -p $(DST)
	@ $(CC) $(CFLAGS) -o $(DST)/$(TARGET) $(SRC) -I $(INCLUDE)

clean:
	rm -f $(TARGET)
