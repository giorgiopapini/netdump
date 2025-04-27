UNAME_S := $(shell uname -s)
SRC := $(wildcard *.c **/*.c ***/**/*.c)
OBJ := $(SRC:.c=.o)

# Compiler and flags
CC := gcc
CFLAGS := -fPIC -g -w
LDFLAGS := -lpcap -lm

# Library source files
LIB_SRC := utils/protocol.c utils/visualizer.c
LIB_OBJ := $(LIB_SRC:.c=.o)
LIB_TARGET := libnetdump.so

# Binary output
TARGET := netdump

# OS-specific variables
ifeq ($(UNAME_S),Linux)
    LIB_PATH := /usr/local/lib64
    RPATH := -Wl,-rpath,\$$ORIGIN:/usr/local/lib64
    INSTALL_LIB_PATH := /usr/local/lib64
    INSTALL_BIN_PATH := /usr/local/bin
endif

ifeq ($(UNAME_S),FreeBSD)
    LIB_PATH := /usr/local/lib
    RPATH := -Wl,-rpath,\$$ORIGIN:/usr/local/lib
    INSTALL_LIB_PATH := /usr/local/lib
    INSTALL_BIN_PATH := /usr/local/bin
endif

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
	$(CC) -o $(TARGET) $(OBJ) $(LDFLAGS) -L. -lnetdump $(RPATH)

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
	mkdir -p $(DESTDIR)$(INSTALL_BIN_PATH)
	mkdir -p $(DESTDIR)$(INSTALL_LIB_PATH)
	install -m 0755 $(TARGET) $(DESTDIR)$(INSTALL_BIN_PATH)/$(TARGET)
	install -m 0755 $(LIB_TARGET) $(DESTDIR)$(INSTALL_LIB_PATH)/$(LIB_TARGET)

# Remove target: remove netdump, libnetdump.so and .o files
remove:
	make clean
	rm -f $(DESTDIR)$(INSTALL_BIN_PATH)/$(TARGET)
	rm -f $(DESTDIR)$(INSTALL_LIB_PATH)/$(LIB_TARGET)
