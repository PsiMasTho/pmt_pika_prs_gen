C_SRC = $(wildcard *.c)
CXX_SRC	= $(wildcard *.cpp)
C_HDR = $(wildcard *.h)
CXX_HDR = $(wildcard *.hpp)

OBJ	= ${C_SRC:.c=.o} ${CXX_SRC:.cpp=.o}

NAME = pmt_util_parse_test_exe

.PHONY: all clean format

all: ${NAME}

clean:
	rm -f ${NAME} ${OBJ}

format:
	${FORMATTER} -i	${C_SRC} ${CXX_SRC} ${C_HDR} ${CXX_HDR}

.c.o:
	${CC} -c ${CFLAGS} ${INCS} $<

.cpp.o:
	${CXX} -c ${CXXFLAGS} ${INCS} $<

${NAME}: ${OBJ}
	${CXX} -o $@ ${OBJ} ${LDFLAGS}