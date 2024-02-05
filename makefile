CC = g++
CFLAGS = -Ofast -Wall -Werror -pthread -pedantic

all: obrc.cpp
	$(CC) $(CFLAGS) -o obrc obrc.cpp

clean:
	rm -rf obrc
