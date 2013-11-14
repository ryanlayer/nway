CC=gcc
SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin
#DFLAGS=-D DEBUGITTER
#DFLAGS=-D IN_TIME_SPLIT
#CFLAGS=-I. -lpthread -g -pg
CFLAGS=-O3
DEPS = 
OBJ = $(OBJ_DIR)/nway.o \
	$(OBJ_DIR)/pq.o \
	$(OBJ_DIR)/timer.o \
	$(OBJ_DIR)/threadpool.o 
EXECS=nway \
	split \
	sweep \
	itter \
	sweepq \
	little_split \
	little_sweep

all: $(EXECS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	[ -d $(OBJ_DIR) ] || mkdir -p $(OBJ_DIR)
	[ -d $(BIN_DIR) ] || mkdir -p $(BIN_DIR)
	$(CC) -g -c -o $@ $< $(CFLAGS) $(DFLAGS)

little_split: little_split.o $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

little_sweep: little_sweep.o $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

sweep: $(OBJ_DIR)/sweep.o $(OBJ)
	gcc -o $(BIN_DIR)/$@ $^ $(CFLAGS)

itter: itter.o $(OBJ)
	gcc -o $@ $^ $(CFLAGS) -lm

nway: nway_main.o $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

split: $(OBJ_DIR)/split.o $(OBJ)
	gcc -o $(BIN_DIR)/$@ $^ $(CFLAGS)

split_o: $(OBJ_DIR)/split_o.o $(OBJ)
	gcc -o $(BIN_DIR)/$@ $^ $(CFLAGS)

psplit: psplit.o $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

set_test: set_test.o $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

gen_set: $(OBJ_DIR)/gen_set.o $(OBJ)
	gcc -o $(BIN_DIR)/$@ $^ $(CFLAGS)

clean:
	rm -rf obj bin
