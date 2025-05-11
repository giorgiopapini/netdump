# Compiler and flags
CC = gcc  # clang is also supported
LDFLAGS = -lpcap -lm
LDFLAGS += -L. -lnetdump

CFLAGS = -g -O2
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
#CFLAGS += -Wlogical-op  # NOT supported on clang
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
SRC != find . -name '*.c' | grep -v 'utils/protocol.c' | grep -v 'utils/visualizer.c'
OBJ = ${SRC:.c=.o}

LIB_SRC = utils/protocol.c utils/visualizer.c
LIB_OBJ = ${LIB_SRC:.c=.o}
LIB_EXT = .so
LIB_TARGET = libnetdump${LIB_EXT}
TARGET = netdump

PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
LIBDIR = $(PREFIX)/lib64

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
	${CC} -fPIC -o $@ ${OBJ} ${LDFLAGS} -Wl,-rpath,\$$ORIGIN:${LIBDIR}

show-config:
	@echo "Configuration:"
	@echo "		PREFIX     = ${PREFIX}"
	@echo "		BINDIR     = ${BINDIR}"
	@echo "		LIBDIR     = ${LIBDIR}"
	@echo "		LIB_EXT    = ${LIB_EXT}"
	@echo "		LIB_TARGET = ${LIB_TARGET}"
	@echo "		CC         = ${CC}"
	@echo "		CFLAGS     = ${CFLAGS}"
	@echo "		LDFLAGS    = ${LDFLAGS}"

# Clean
clean:
	rm -f ${TARGET} ${LIB_TARGET}
	find . -name '*.o' -exec rm -f {} +

# Install
install: ${TARGET} ${LIB_TARGET}
	@test -d ${BINDIR} || (echo "Error: BINDIR '${BINDIR}' does not exist. Set it correctly."; exit 1)
	@test -d ${LIBDIR} || (echo "Error: LIBDIR '${LIBDIR}' does not exist. Set it correctly."; exit 1)

	@echo "Installing netdump (binaries) to ${DESTDIR}${BINDIR}"
	install -m 0755 ${TARGET} ${DESTDIR}${BINDIR}/${TARGET}
	
	@echo "Installing ${LIB_TARGET} (shared lib) to ${DESTDIR}${LIBDIR}"
	install -m 0755 ${LIB_TARGET} ${DESTDIR}${LIBDIR}/${LIB_TARGET}

# Remove
remove: clean
	rm -f ${DESTDIR}${BINDIR}/${TARGET}
	rm -f ${DESTDIR}${LIBDIR}/${LIB_TARGET}