ROOT_DIR := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))

FORMATTER	= clang-format-18

CC	    = gcc
CFLAGS  = -std=c99 -Wall -Wextra -pedantic -O0 -ggdb3 -fsanitize=address,undefined
LDFLAGS	= -fsanitize=address,undefined
INCS    = -I${ROOT_DIR}src

export ROOT_DIR FORMATTER CC CFLAGS LDFLAGS INCS