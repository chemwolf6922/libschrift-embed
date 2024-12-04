# Customize below to fit your system

# compiler and linker
CC = cc
LD = cc
AR = ar
RANLIB = ranlib

# installation paths
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man

# compiler flags for libschrift
CPPFLAGS =
CFLAGS   = -Os -std=c99 -pedantic -Wall -Wextra -Wconversion
LDFLAGS  = -Os

# compiler flags for the demo & stress test applications
EXTRAS_CPPFLAGS = -I./
EXTRAS_CFLAGS   = -g -Os -std=c99 -pedantic -Wall -Wextra
EXTRAS_LDFLAGS  = -g -Os

