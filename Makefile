CC=gcc
CLINKER=gcc
CCFLAGS=-c -Wall -O3 -std=gnu99
CLOPT=
SOURCES=src/main.c src/binary_heap.c src/huffman_encoding.c src/huffman_tree.c src/bitset.c
OBJ=$(SOURCES:.c=.o)
EXEC=huffman_encoding

all:	$(SOURCES) $(EXEC)

$(EXEC): $(OBJ)
	$(CLINKER) $(CLOPT) $(OBJ) -o $@

.c.o:
	$(CC) $(CCFLAGS) $< -o $@

clean:
	rm -f $(OBJ)
	rm -f $(EXEC)
