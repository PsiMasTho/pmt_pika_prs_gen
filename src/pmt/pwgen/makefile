SRC = $(wildcard *.c)
OBJ = ${SRC:.c=.o}
HDR = $(wildcard *.h)

NAME = pmt_pwgen

.PHONY: all clean format

all: ${NAME}

clean:
	rm -f ${NAME} ${OBJ}

format:
	${FORMATTER} -i ${SRC} ${HDR}

.c.o:
	${CC} -c ${CFLAGS} ${INCS} $<

${NAME}: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}
