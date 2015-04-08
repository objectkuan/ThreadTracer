CC=gcc
OUTDIR=bin
FLAGS=-g -pthread

all: 
	$(CC) -I./include $(FLAGS) filter.c $(shell find modules -name '*.c') -o $(OUTDIR)/filter
	$(CC) -I./include $(FLAGS) samples/tmut.c -o $(OUTDIR)/samples/tmut
	$(CC) -I./include $(FLAGS) samples/3on6.c -o $(OUTDIR)/samples/3on6
	$(CC) -I./include $(FLAGS) samples/deadlock.c -o $(OUTDIR)/samples/deadlock
