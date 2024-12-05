# See LICENSE file for copyright and license details.

CC?=gcc
AR?=ar

CFLAGS?=-Os
override CFLAGS+=-std=c99 -pedantic -Wall -Wextra -Wconversion -MMD -MP
ifneq ($(SFT_USE_FLOAT), )
override CFLAGS+=-DSFT_USE_FLOAT
endif
ifneq ($(SFT_DEBUG), )
override CFLAGS+=-DSFT_DEBUG  -g -O0
endif

LIB=../libschrift.a

DEMO=demo
DEMO_SRC=demo.c util.c

STRESS=stress
STRESS_SRC=stress.c util.c

SINGLE=single_char
SINGLE_SRC=single_char.c util.c

.PHONY: all clean

all: $(DEMO) $(STRESS) $(SINGLE)

demo: $(DEMO_SRC:.c=.o) $(LIB)
	$(CC) $(LDFLAGS) -o $@ $^ -lm

stress: $(STRESS_SRC:.c=.o) $(LIB)
	$(CC) $(LDFLAGS) -o $@ $^ -lm

single_char: $(SINGLE_SRC:.c=.o) $(LIB)
	$(CC) $(LDFLAGS) -o $@ $^ -lm

%.o:%.c
	$(CC) -c $(CFLAGS) -o $@ $<

-include $(DEMO_SRC:.c=.d) $(STRESS_SRC:.c=.d)

clean:
	rm -f *.o *.d *.ppm
	rm -f $(DEMO) $(STRESS) $(SINGLE)
