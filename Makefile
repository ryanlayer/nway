CC=gcc
#DFLAGS=-D DEBUGITTER
CFLAGS=-I.
DEPS = 
OBJ = nway.o \
	pq.o \
	timer.o \
	threadpool.o 
EXECS=nway \
	split \
	sweep \
	itter \
	sweepq \
	little_split \
	little_sweep

all: $(EXECS)

%.o: %.c $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS) $(DFLAGS)

little_split: little_split.o $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

little_sweep: little_sweep.o $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

sweep: sweep.o $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

itter: itter.o $(OBJ)
	gcc -o $@ $^ $(CFLAGS) -lm

nway: nway_main.o $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

split: split.o $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

psplit: psplit.o $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

set_test: set_test.o $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

gen_set: gen_set.o $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o $(EXECS)
