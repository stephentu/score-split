CC=gcc

all: score-split

score-split: score-split.c
	$(CC) -Wall -Werror -O2 -o score-split score-split.c

.PHONY: clean
clean: 
	rm -f score-split
