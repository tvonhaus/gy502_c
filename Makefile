CC = gcc
CFLAGS =
TARG = gy502
LIB = -li2c
SRC = gy502.c
OBJ = $(SRC:.c=.o)

all: $(TARG)

$(TARG): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LIB)

%.o : %.c
	$(CC) -c $<	

clean:
	rm -f $(OBJ) $(TARG)


