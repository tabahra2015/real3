# Define the compiler
CC = gcc

# Define compiler flags
CFLAGS = -Wall -Wextra -O2

# Define the source files and the target executable
SRC = parent.c resistance.c civilian.c agency.c enemy.c
OBJ = parent.o resistance.o civilian.o agency.o enemy.o
TARGET = program

# Default target to compile the program
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) -lGL -lGLU -lglut -lpthread -lm  # Link OpenGL, pthread, and math libraries

# Compile source files into object files
parent.o: parent.c
	$(CC) $(CFLAGS) -c parent.c

resistance.o: resistance.c resistance.h
	$(CC) $(CFLAGS) -c resistance.c

civilian.o: civilian.c civilian.h
	$(CC) $(CFLAGS) -c civilian.c

agency.o: agency.c agency.h
	$(CC) $(CFLAGS) -c agency.c

enemy.o: enemy.c enemy.h
	$(CC) $(CFLAGS) -c enemy.c

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean up the generated files
clean:
	rm -f $(TARGET) $(OBJ)

# Phony targets so Make doesn't confuse them with files
.PHONY: all run clean
