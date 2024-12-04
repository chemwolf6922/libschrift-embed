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

DEMO=demo
DEMO_SRC=demo.c

STRESS=stress
STRESS_SRC=stress.c

.PHONY: all clean

all: $(LIB) $(DEMO) $(STRESS)

$(LIB): $(LIB_SRC:.c=.o)
	$(AR) rc $@ $^
	$(RANLIB) $@

demo: $(DEMO_SRC:.c=.o) $(LIB)
	$(CC) $(LDFLAGS) -o $@ $^ -lm

stress: $(STRESS_SRC:.c=.o) $(LIB)
	$(CC) $(LDFLAGS) -o $@ $^ -lm

%.o:%.c
	$(CC) -c $(CFLAGS) -o $@ $<

-include $(LIB_SRC:.c=.d) $(DEMO_SRC:.c=.d) $(STRESS_SRC:.c=.d)

clean:
	rm -f *.o
	rm -f util/*.o
	rm -f libschrift.a
	rm -f demo
	rm -f stress

