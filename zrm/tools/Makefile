all: index symdex gloss

INDEX = index.o common.o
index: $(INDEX)
	gcc -o index $(INDEX)

SYMDEX = symdex.o common.o
symdex: $(SYMDEX)
	gcc -o symdex $(SYMDEX)

GLOSS = gloss.o common.o
gloss: $(GLOSS)
	gcc -o gloss $(GLOSS)

index.o symdex.o gloss.o common.o: common.h

CC = gcc
