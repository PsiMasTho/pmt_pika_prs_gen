C_SRC = $(wildcard *.c)
CXX_SRC	= $(wildcard *.cpp)
C_HDR = $(wildcard *.h)
CXX_HDR = $(wildcard *.hpp)

OBJ	= ${C_SRC:.c=.o} ${CXX_SRC:.cpp=.o}

NAME = pmt_util_parse_test_exe

LD_DIRS_FLAGS	= -L${ROOT_DIR}src/pmt/util/parse/test/ -L${ROOT_DIR}src/pmt/util/parse/ -L${ROOT_DIR}src/pmt/util/ -L${ROOT_DIR}src/pmt/base/ -L${ROOT_DIR}src/pmt/
LD_LNK_FLAGS	= -lpmt_util_parse_test -lpmt_util_parse -lpmt_util -lpmt_base -lpmt

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
	${CXX} -o $@ ${OBJ} ${LDFLAGS} ${LD_DIRS_FLAGS} ${LD_LNK_FLAGS}