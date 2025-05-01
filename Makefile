# Compiler and flags
CC ?= gcc
LDFLAGS ?= -lpcap -lm
CFLAGS ?= -fPIC -g
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

# Default target
all: ${LIB_TARGET} ${TARGET}

# Compile object files
.c.o:
	${CC} ${CFLAGS} -c $< -o $@

# Create shared library
${LIB_TARGET}: ${LIB_OBJ}
	${CC} -shared -o $@ ${LIB_OBJ} -fPIC

# Link target with shared library
${TARGET}: ${OBJ} ${LIB_TARGET}
	${CC} -o $@ ${OBJ} -L. -lnetdump ${LDFLAGS} -Wl,-rpath,\$$ORIGIN:/usr/local/lib64

# Clean
clean:
	rm -f ${TARGET} ${LIB_TARGET}
	find . -name '*.o' -exec rm -f {} +

# Debug
debug: clean ${TARGET}
	@echo "Running ${TARGET}..."
	sudo ./${TARGET}

# Install
install: ${TARGET} ${LIB_TARGET}
	mkdir -p ${DESTDIR}/usr/local/bin
	mkdir -p ${DESTDIR}/usr/local/lib64
	install -m 0755 ${TARGET} ${DESTDIR}/usr/local/bin/${TARGET}
	install -m 0755 ${LIB_TARGET} ${DESTDIR}/usr/local/lib64/${LIB_TARGET}

# Remove
remove: clean
	rm -f ${DESTDIR}/usr/local/bin/${TARGET}
	rm -f ${DESTDIR}/usr/local/lib64/${LIB_TARGET}