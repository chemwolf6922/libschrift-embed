# See LICENSE file for copyright and license details.

VERSION=0.10.2

CC?=gcc
AR?=ar
RANLIB?=ranlib

CFLAGS?=-Os
override CFLAGS+=-std=c99 -pedantic -Wall -Wextra -Wconversion -MMD -MP
ifneq ($(SFT_USE_FLOAT), )
override CFLAGS+=-DSFT_USE_FLOAT
endif
ifneq ($(SFT_DEBUG), )
override CFLAGS+=-DSFT_DEBUG -g -O0
endif

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
	$(CC) -c $(CFLAGS) -DSCHRIFT_VERSION="\"$(VERSION)\"" -o $@ $<

-include $(LIB_SRC:.c=.d)

clean:
	rm -f *.o *.d
	rm -f $(LIB)
	$(MAKE) -C test clean
