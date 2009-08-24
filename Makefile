.PHONY: all test depend clean

CC       = gcc
INCLUDES = -g
CFLAGS   = $(INCLUDES) -Wall -W -pedantic -std=c99
LIBS     = 

EXE      = myforth
SRC      = main.c forth.c util.c stack.c
OBJS     = $(SRC:.c=.o)



all: $(EXE)

# link
$(EXE): $(OBJS)
	$(CC) $(LIBS) -o $(EXE) $(OBJS) $(LIBS)

# compile
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

# test
test: $(OBJS)
	# stack test
	cd t && gcc -g stack-test.c -o stack-test ../stack.o
	t/stack-test

# depend
depend:
	\makedepend -- $(CFLAGS) -- $(SRC)

# clean
clean:
	\rm -f $(OBJS) $(EXE)
