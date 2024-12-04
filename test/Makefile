# See LICENSE file for copyright and license details.

.POSIX:

CC?=gcc
AR?=ar

CFLAGS?=-Os
override CFLAGS+=-std=c99 -pedantic -Wall -Wextra -Wconversion -MMD -MP

LIB=../libschrift.a

DEMO=demo
DEMO_SRC=demo.c util.c

STRESS=stress
STRESS_SRC=stress.c util.c

.PHONY: all clean

all: $(DEMO) $(STRESS)

demo: $(DEMO_SRC:.c=.o) $(LIB)
	$(CC) $(LDFLAGS) -o $@ $^ -lm

stress: $(STRESS_SRC:.c=.o) $(LIB)
	$(CC) $(LDFLAGS) -o $@ $^ -lm

%.o:%.c
	$(CC) -c $(CFLAGS) -o $@ $<

-include $(DEMO_SRC:.c=.d) $(STRESS_SRC:.c=.d)

clean:
	rm -f *.o *.d *.ppm
	rm -f $(DEMO) $(STRESS)
