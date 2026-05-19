MAKE=gcc
CFLAGS=-std=c11 -Wall -Wextra

CFLAG=

ABSDIR=$(shell pwd)
PROM=multicast_test
DEPS=$(shell find ${ABSDIR} -type f -name "*.h") $(shell find ${ABSDIR}/../common  -type f -name "*.h")
SRCS=$(shell find ${ABSDIR} -type f -name "*.c") $(shell find ${ABSDIR}/../common  -type f -name "*.c")
OBJS=$(patsubst %.c,%.o,${SRCS})

${PROM}: ${OBJS}
    ${MAKE} ${CFLAGS} -o ${PROM} $^ ${CFLAG}

%.o: %.c ${DEPS}
    ${MAKE} ${CFLAGS} -o $@ -c $< ${CFLAG}

clean:
    rm -rf ${PROM} ${OBJS}
