# Para ver información sobre a utilización ver `src/main.c`

CFLAGS = -Wall -std=c2x
TARGET = practica_01
DIRS = src src/tipos src/estados
SRC = $(foreach d, $(DIRS), $(wildcard $(d)/*.c))
INCLUDE = src
DST = bin

all: $(TARGET)

# Activa a saida de debug do programa (moito menos optimizado)
debug: CFLAGS += -DDEBUG
debug: $(TARGET)

# Activa o visor do buffer de dobre centinela (además de debug)
centinela: CFLAGS += -DDEBUG_CENTINELA -DDEBUG
centinela: $(TARGET)

# Compila e executa o programa
run: $(TARGET)
	@ $(DST)/$(TARGET)

$(TARGET): $(SRC)
	@ mkdir -p $(DST)
	@ $(CC) $(CFLAGS) -o $(DST)/$(TARGET) $(SRC) -I $(INCLUDE)

clean:
	rm -f $(TARGET)
