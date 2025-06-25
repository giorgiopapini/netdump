# Compiler and flags
# clang is also supported
CC ?= gcc
LDFLAGS += -lpcap -lm
LDFLAGS += -L. -lnetdump

CFLAGS += -g -O2
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
#CFLAGS += -Wformat=2
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

UNAME_S := $(shell uname -s)

# Source files (recursive find using shell, for portability)
ifeq ($(origin MAKE_VERSION), undefined)
  # BSD Make
  SRC != find . -name '*.c' | grep -v 'utils/protocol.c' | grep -v 'utils/visualizer.c'
  OBJ = ${SRC:.c=.o}
else
  # GNU Make
  SRC := $(shell find . -name '*.c' | grep -v 'utils/protocol.c' | grep -v 'utils/visualizer.c')
  OBJ = $(SRC:%.c=%.o)
endif

LIB_SRC = utils/protocol.c utils/visualizer.c
LIB_OBJ = ${LIB_SRC:.c=.o}

ifeq ($(UNAME_S),Darwin)
	LIB_EXT = .dylib
	SHAREDFLAG = -dynamiclib
	INSTALLNAME = -Wl,-install_name,$(PREFIX)/lib/$(LIB_TARGET)
else
	LIB_EXT = .so
	SHAREDFLAG = -shared
	RPATH = -Wl,-rpath,\$$ORIGIN:${LIBDIR}
endif

LIB_TARGET = libnetdump${LIB_EXT}
TARGET = netdump

PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin

ifeq ($(UNAME_S),Linux)
	LIBDIR = $(PREFIX)/lib64
else
	LIBDIR = $(PREFIX)/lib
endif

# Default target
all: ${TARGET} ${LIB_TARGET}

utils/protocol.o: utils/protocol.c
	${CC} ${CFLAGS} -fPIC -c $< -o $@

utils/visualizer.o: utils/visualizer.c
	${CC} ${CFLAGS} -fPIC -c $< -o $@

# Compile object files
%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@

# Create shared library
${LIB_TARGET}: ${LIB_OBJ}
	${CC} ${SHAREDFLAG} -o $@ ${LIB_OBJ} ${INSTALLNAME}

# Link target with shared library
${TARGET}: ${OBJ} ${LIB_TARGET}
	${CC} -o $@ ${OBJ} ${LDFLAGS} ${RPATH}

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
