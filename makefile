CC = g++
CFLAGS = -Wall -Werror -pedantic

all: obrc.cpp
	$(CC) $(CFLAGS) -o obrc obrc.cpp

clean:
	rm -rf obrc
