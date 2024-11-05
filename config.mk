FORMATTER	= clang-format-16

RAGEL = ragel
LEMON = lemon--
CXX	  = clang++-16
#CXX         = g++-14

#	Debug
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic -O0 -ggdb3 -fsanitize=address,undefined
LDFLAGS	 = -fsanitize=address,undefined

#CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic -O0 -ggdb3
#LDFLAGS	 = 

#	Release
#CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic -O3 -march=native -DNDEBUG -flto
#LDFLAGS	 = -flto

export FORMATTER CXX CXXFLAGS LDFLAGS
