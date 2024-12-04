# See LICENSE file for copyright and license details.

.POSIX:

VERSION=0.10.2

CC?=gcc
AR?=ar
RANLIB?=ranlib

CFLAGS?=-Os
override CFLAGS+=-std=c99 -pedantic -Wall -Wextra -Wconversion -MMD -MP

LIB=libschrift.a
LIB_SRC=schrift.c

.PHONY: all lib test clean

all: lib test

lib: $(LIB)

test: lib
	$(MAKE) -C test

$(LIB): $(LIB_SRC:.c=.o)
	$(AR) rc $@ $^
	$(RANLIB) $@

%.o:%.c
	$(CC) -c $(CFLAGS) -o $@ $<

-include $(LIB_SRC:.c=.d)

clean:
	rm -f *.o *.d
	rm -f $(LIB)
	$(MAKE) -C test clean
