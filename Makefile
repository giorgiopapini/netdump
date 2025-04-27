SRC := $(wildcard *.c **/*.c ***/**/*.c)
OBJ := $(SRC:.c=.o)

# Compiler and flags
CC := gcc
CFLAGS := -fPIC -g -w
LDFLAGS := -lpcap -lm

# Library source files
LIB_SRC := utils/protocol.c utils/visualizer.c protocols/proto_tables_nums.c
LIB_OBJ := $(LIB_SRC:.c=.o)
LIB_TARGET := libnetdump.so

# Binary output
TARGET := netdump

# Default target: build everything
all: $(LIB_TARGET) $(TARGET)

# Compile object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Create shared library
$(LIB_TARGET): $(LIB_OBJ)
	$(CC) -shared -o $@ $^ -fPIC

# Compile netdump, linking dynamically to libnetdump.so
$(TARGET): $(OBJ) $(LIB_TARGET)
	$(CC) -o $(TARGET) $(OBJ) -L. -lnetdump $(LDFLAGS) -Wl,-rpath,\$$ORIGIN:/usr/local/lib64

# Clean target
clean:
	-rm -f $(TARGET) $(LIB_TARGET) *.o **/*.o ***/**/*.o

# Run netdump with sudo
run: $(TARGET)
	sudo ./$(TARGET)

# Debug target: recompile, clear screen, and run the program
debug: clean $(TARGET)
	clear
	sudo ./$(TARGET)

# Install target: install netdump and libnetdump.so
install: $(TARGET) $(LIB_TARGET)
	mkdir -p $(DESTDIR)/usr/local/bin
	mkdir -p $(DESTDIR)/usr/local/lib64
	install -m 0755 $(TARGET) $(DESTDIR)/usr/local/bin/$(TARGET)
	install -m 0755 $(LIB_TARGET) $(DESTDIR)/usr/local/lib64/$(LIB_TARGET)