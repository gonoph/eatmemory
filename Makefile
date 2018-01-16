.PHONY: all

CFLAGS := -Wall -Os

all: eatmemory

clean:
	$(RM) -fv eatmemory eatmemory.o
	
dist-clean: clean
	$(RM) -fv *~

eatmemory.o: eatmemory.c

eatmemory: eatmemory.o
	$(CC) $@.o -o $@
