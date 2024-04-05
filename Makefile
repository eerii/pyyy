# Para ver información sobre a utilización ver `src/main.c`

LEX = flex
LFLAGS =
CFLAGS = -lfl -std=c2x
DIRS = src
DST = bin
TARGET = practica_02
SRC_LEX = $(foreach d, $(DIRS), $(wildcard $(d)/*.l))
OUT_FLEX = $(patsubst $(DIRS)/%.l, $(DST)/flex/%.yy.c, $(SRC_LEX))
SRC = $(OUT_FLEX) $(foreach d, $(DIRS), $(wildcard $(d)/*.c))
INCLUDE = src

all: $(TARGET)

# Activa a saida de debug do programa (moito menos optimizado)
debug: CFLAGS += -DDEBUG
debug: $(TARGET)

# Compila e executa o programa
run: $(TARGET)
	@ $(DST)/$(TARGET)

$(TARGET): $(SRC)
	@ mkdir -p $(DST)
	@ $(CC) $(CFLAGS) -o $(DST)/$(TARGET) $(SRC) -I $(INCLUDE)

# Utiliza flex para convertir os arquivos lex en src/*.l en arquivos c en bin/flex/*.yy.c
$(DST)/flex/%.yy.c: $(DIRS)/%.l
	@ mkdir -p $(DST)/flex
	@ $(LEX) -o $@ $(LFLAGS) $<

$(DST)/flex/%.yy.h: $(DIRS)/%.l
	@ mkdir -p $(DST)/flex
	@ $(LEX) -o $@ $(LFLAGS) $<

clean:
	rm -f $(TARGET)
