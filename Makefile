CC=gcc
CFLAGS=-I.
DEPS = 
OBJ = nway.o \
	  pq.o \
	  timer.o
EXECS=nway \
	  split \
	  sweep \
	  sweepq 

all: $(EXCECS)

%.o: %.c $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS)

nway: nway_main.o $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

split: nway_split.o $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

sweep: nway_sweep.o $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

sweepq: nway_sweepq.o $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm *.o 
