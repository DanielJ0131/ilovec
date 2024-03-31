# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Werror

# Libraries
LIBS = -lSDL2 -lSDL2_mixer -lSDL2_ttf -lSDL2_image

# Source file
SOURCE = tg.c

# Executable name
EXECUTABLE = a.out

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) $(LIBS) -o $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)