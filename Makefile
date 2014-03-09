CFLAGS += -std=c99 -g -Wall -Werror

all: clean run

test: stp_print.o test.o
	$(CC) -o $@  $^

run: test
	./test

clean:
	rm -f *.o *~ test
