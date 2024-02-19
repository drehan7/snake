CC=gcc
CFLAGS=-Wall -pedantic -Wextra -fcommon
LDFLAGS=-Isrc -lSDL2
SRC=src
SOURCES=$(wildcard $(SRC)/*.c)
HEADERS=$(wildcard $(SRC)/*.h)
OBJ=Objs
OBJECTS=$(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(SOURCES))
TARGET=game

build: $(TARGET)

clean:
	rm -r $(OBJ)/*
	rm $(TARGET)
	@#

run: $(TARGET)
	@./game
	
.PHONY: build clean run

##

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(CFLAGS) $^ -o $@
	@echo "Build Successful!"

$(OBJECTS): $(SOURCES) $(HEADERS)
	@echo Compiling $^
	# FILE=$(patsubst $(OBJ)/%.o,$(SRC)/%.c,$@)
	# @mkdir -p $(@D)
	# $(CC) $(CFLAGS) $< -o $@
	$(CC) $(CFLAGS) -c $(patsubst $(OBJ)/%.o,$(SRC)/%.c,$@) -o $@
