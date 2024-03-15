# Compiler
CC = gcc

# Compiler flags
CFLAGS = -g -Werror

# Source files
SRCS = main.c

# Object files
OBJS = list.o

# Executable
TARGET = main

all: $(TARGET)

$(TARGET): $(SRCS) $(OBJS)
	$(CC) $(CFLAGS) $(SRCS) $(OBJS) -o $(TARGET)

clean:
	rm -f $(TARGET)
