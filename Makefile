CFLAGS = -Wall -std=c2x
TARGET = practica_01
SRC = $(wildcard src/*.c) $(wildcard src/tad/*.c)
INCLUDE = src
DST = bin

all: $(TARGET)

$(TARGET): $(SRC)
	@ mkdir -p $(DST)
	@ $(CC) $(CFLAGS) -o $(DST)/$(TARGET) $(SRC) -I $(INCLUDE)

run: $(TARGET)
	@ $(DST)/$(TARGET)

clean:
	rm -f $(TARGET)
