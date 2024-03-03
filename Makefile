CC = gcc
CFLAGS = -Wall -std=c2x
TARGET = practica_01
DIRS = src src/tipos src/estados
SRC = $(foreach d, $(DIRS), $(wildcard $(d)/*.c))
INCLUDE = src
DST = bin

all: $(TARGET)

debug: CFLAGS += -DDEBUG
debug: $(TARGET)

$(TARGET): $(SRC)
	@ mkdir -p $(DST)
	@ $(CC) $(CFLAGS) -o $(DST)/$(TARGET) $(SRC) -I $(INCLUDE)

run: $(TARGET)
	@ $(DST)/$(TARGET)

clean:
	rm -f $(TARGET)
