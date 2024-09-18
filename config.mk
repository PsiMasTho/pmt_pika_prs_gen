ROOT_DIR := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))

FORMATTER	= clang-format-18

CXX	     = clang++-18
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic -O0 -ggdb3 -fsanitize=address,undefined
CFLAGS   = -std=c99   -Wall -Wextra -pedantic -O0 -ggdb3 -fsanitize=address,undefined
LDFLAGS	 = -fsanitize=address,undefined
INCS     = -I${ROOT_DIR}src

export ROOT_DIR FORMATTER CXX CXXFLAGS CC CFLAGS LDFLAGS INCS