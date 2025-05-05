# Compiler and flags
CC = gcc
LDFLAGS = -lpcap -lm
LDFLAGS += -L. -lnetdump

CFLAGS = -g
#CFLAGS += -std=c99
#CFLAGS += -Wpedantic -pedantic-errors
CFLAGS += -Werror
CFLAGS += -Wall
CFLAGS += -Wextra
#CFLAGS += -Waggregate-return
CFLAGS += -Wbad-function-cast
CFLAGS += -Wcast-align
CFLAGS += -Wcast-qual
CFLAGS += -Wdeclaration-after-statement
CFLAGS += -Wfloat-equal
CFLAGS += -Wformat=2
#CFLAGS += -Wlogical-op
CFLAGS += -Wmissing-declarations
CFLAGS += -Wmissing-include-dirs
CFLAGS += -Wmissing-prototypes
CFLAGS += -Wnested-externs
CFLAGS += -Wpointer-arith
CFLAGS += -Wredundant-decls
CFLAGS += -Wsequence-point
CFLAGS += -Wshadow
CFLAGS += -Wstrict-prototypes
CFLAGS += -Wswitch
CFLAGS += -Wundef
CFLAGS += -Wunreachable-code
CFLAGS += -Wunused-but-set-parameter
CFLAGS += -Wwrite-strings
CFLAGS += -Wconversion -Wsign-conversion
# Only for debugging!
#LDFLAGS += -fsanitize=address,undefined
#CFLAGS += -fsanitize=address,undefined
#CFLAGS += -O0

# Source files (recursive find using shell, for portability)
SRC != find . -name '*.c'
OBJ = ${SRC:.c=.o}

LIB_SRC = utils/protocol.c utils/visualizer.c
LIB_OBJ = ${LIB_SRC:.c=.o}
LIB_TARGET = libnetdump.so
TARGET = netdump

PREFIX = /usr/local
BIN_DIR = $(PREFIX)/bin
LIB_DIR = $(PREFIX)/lib64

# Default target
all: ${TARGET} ${LIB_TARGET}

# Compile object files
.c.o:
	${CC} -fPIC ${CFLAGS} -c $< -o $@

# Create shared library
${LIB_TARGET}: ${LIB_OBJ}
	${CC} -shared -fPIC -o $@ ${LIB_OBJ}

# Link target with shared library
${TARGET}: ${OBJ} ${LIB_TARGET}
	${CC} -fPIC -o $@ ${OBJ} ${LDFLAGS} -Wl,-rpath,\$$ORIGIN:${LIB_DIR}

# Clean
clean:
	rm -f ${TARGET} ${LIB_TARGET}
	find . -name '*.o' -exec rm -f {} +

# Install
install: ${TARGET} ${LIB_TARGET}
	mkdir -p ${DESTDIR}${BIN_DIR}
	mkdir -p ${DESTDIR}${LIB_DIR}

	@echo "Installing netdump (binaries) to ${DESTDIR}${BIN_DIR}"
	install -m 0755 ${TARGET} ${DESTDIR}${BIN_DIR}/${TARGET}
	
	@echo "Installing ${LIB_TARGET} (shared lib) to ${DESTDIR}${LIB_DIR}"
	install -m 0755 ${LIB_TARGET} ${DESTDIR}${LIB_DIR}/${LIB_TARGET}

# Remove
remove: clean
	rm -f ${DESTDIR}${BIN_DIR}/${TARGET}
	rm -f ${DESTDIR}${LIB_DIR}/${LIB_TARGET}