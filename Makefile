# Automatically include all .c files in the current directory and subdirectories
SRC := $(wildcard *.c) $(wildcard **/*.c) $(wildcard ***/**/*.c)

# Compiler and flags
CC := gcc
CFLAGS := -g -w
LDFLAGS := -lpcap

# Output binary
TARGET := netdump

# Default target: compile the program
all: $(TARGET)

# Compile target
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# Clean target: remove the binary
clean:
	-rm -f $(TARGET)

# Run target: execute the program with sudo
run: $(TARGET)
	sudo ./$(TARGET)

# Debug target: recompile, clear screen, and run the program
debug: clean $(TARGET)
	clear
	sudo ./$(TARGET)

# Install target: install the binary to the system's bin directory
install: $(TARGET)
	mkdir -p $(DESTDIR)/usr/bin
	install -m 0755 $(TARGET) $(DESTDIR)/usr/bin/$(TARGET)
