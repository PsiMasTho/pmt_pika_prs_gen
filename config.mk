FORMATTER	= clang-format-18

CXX	     = clang++-18

#	Debug
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic -O0 -ggdb3 -fsanitize=address,undefined
LDFLAGS	 = -fsanitize=address,undefined

#	Release
#CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic -O3 -march=native -DNDEBUG

export FORMATTER CXX CXXFLAGS LDFLAGS