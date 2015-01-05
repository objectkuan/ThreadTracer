CC=gcc
OUTDIR=bin

all: 
	$(CC) -I./include -g -pthread filter.c modules/ttevent.c modules/ttthread_state.c modules/ttdepsolver.c modules/ttparser.c -o $(OUTDIR)/filter
	$(CC) -I./include -g -pthread tmut.c -o $(OUTDIR)/tmut
	$(CC) -I./include -g -pthread 3on6.c -o $(OUTDIR)/3on6
	$(CC) -I./include -g -pthread deadlock.c -o $(OUTDIR)/deadlock
