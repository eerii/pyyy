# Para ver información sobre a utilización ver `src/main.c`

CFLAGS = -lfl -lm -std=c2x
DIRS = src
DST = bin
TARGET = practica_03

LEX = flex
LFLAGS =
SRC_LEX = $(foreach d, $(DIRS), $(wildcard $(d)/*.l))
OUT_LEX = $(patsubst $(DIRS)/%.l, $(DST)/%.yy.c, $(SRC_LEX))

SIN = bison
SFLAGS = -Wnone -d
SRC_BISON = $(foreach d, $(DIRS), $(wildcard $(d)/*.y))
OUT_BISON = $(patsubst $(DIRS)/%.y, $(DST)/%.tab.c, $(SRC_BISON))

SRC = $(OUT_LEX) $(OUT_BISON) $(foreach d, $(DIRS), $(wildcard $(d)/*.c))
INCLUDE = -I$(DIRS) -I$(DST)

all: $(TARGET)

# Activa a saida de debug do programa (moito menos optimizado)
debug: CFLAGS += -DDEBUG
debug: $(TARGET)

# Compila e executa o programa
run: $(TARGET)
	@ $(DST)/$(TARGET)

$(TARGET): $(SRC)
	@ mkdir -p $(DST)
	@ $(CC) $(CFLAGS) -o $(DST)/$(TARGET) $(SRC) $(INCLUDE)

# Utiliza flex para convertir os arquivos en src/*.l en arquivos c en bin/*.yy.c
$(DST)/%.yy.c: $(DIRS)/%.l
	@ mkdir -p $(DST)
	@ $(LEX) -o $@ --header-file=$(DST)/$*.yy.h $(LFLAGS) $<

# Utiliza bison para convertir os arquivos en src/*.y en arquivos c en bin/*.tab.c
$(DST)/%.tab.c: $(DIRS)/%.y
	@ mkdir -p $(DST)
	@ $(SIN) -o $@ --header=$(DST)/$*.tab.h $(SFLAGS) $<


clean:
	rm -f $(TARGET)
