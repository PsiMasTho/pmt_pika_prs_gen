SRC = $(wildcard *.c)
HDR = $(wildcard *.h)
OBJ = ${SRC:.c=.o}

NAME = generator

.PHONY: all clean format 

all : ${NAME}

clean:
	rm -f ${NAME} ${OBJ}

format:
	${FORMATTER} -i ${SRC} ${HDR}

.c.o:
	${CC} -c ${CFLAGS} ${INCS} $<

${NAME}: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}