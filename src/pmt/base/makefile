SRC = $(wildcard *.c)
OBJ = ${SRC:.c=.o}
HDR = $(wildcard *.h)

NAME = libpmt_base.a

.PHONY: all clean format

all: ${NAME}

clean:
	rm -f ${NAME} ${OBJ}

format:
	${FORMATTER} -i ${SRC} ${HDR}

.c.o:
	${CC} -c ${CFLAGS} ${INCS} $<

${NAME}: ${OBJ}
	ar rcs $@ ${OBJ}