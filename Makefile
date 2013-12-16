CC=gcc
SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin
#DFLAGS=-D SPLIT_SIZES
#DFLAGS=-D DEBUGITTER
#DFLAGS=-D IN_TIME_SPLIT -D RUN_GET_CENTER_SPLIT_TIME
#DFLAGS=-D IN_TIME_SPLIT
#DFLAGS=-D SPLIT_DEBUG
#CFLAGS=-I. -lpthread -g -pg
CFLAGS=-O3 -pthread -ltcmalloc
#CFLAGS=-g
DEPS = 
OBJ = $(OBJ_DIR)/nway.o \
	$(OBJ_DIR)/pq.o \
	$(OBJ_DIR)/timer.o \
	$(OBJ_DIR)/threadpool.o \
	$(OBJ_DIR)/mslab.o \
	$(OBJ_DIR)/utils.o 
EXECS=split \
	split_o \
	psplit_o \
	split_sweep \
	splitn_sweep \
	sweep \
	split_centers \
	psplit_centers \
	gen_set \
	split_psweep

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

psplit_o: $(OBJ_DIR)/psplit_o.o $(OBJ)
	gcc -o $(BIN_DIR)/$@ $^ $(CFLAGS)

split_sweep: $(OBJ_DIR)/split_sweep.o $(OBJ)
	gcc -o $(BIN_DIR)/$@ $^ $(CFLAGS)

splitn_sweep: $(OBJ_DIR)/splitn_sweep.o $(OBJ)
	gcc -o $(BIN_DIR)/$@ $^ $(CFLAGS)

split_psweep: $(OBJ_DIR)/split_psweep.o $(OBJ)
	gcc -o $(BIN_DIR)/$@ $^ $(CFLAGS)

split_centers: $(OBJ_DIR)/split_centers.o $(OBJ)
	gcc -o $(BIN_DIR)/$@ $^ $(CFLAGS)

psplit_centers: $(OBJ_DIR)/psplit_centers.o $(OBJ)
	gcc -o $(BIN_DIR)/$@ $^ $(CFLAGS)

psplit: psplit.o $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

set_test: set_test.o $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

gen_set: $(OBJ_DIR)/gen_set.o $(OBJ)
	gcc -o $(BIN_DIR)/$@ $^ $(CFLAGS)

open_bed: $(OBJ_DIR)/open_bed.o $(OBJ)
	gcc -o $(BIN_DIR)/$@ $^ $(CFLAGS)

get_l1: $(OBJ_DIR)/get_l1.o $(OBJ)
	gcc -o $(BIN_DIR)/$@ $^ $(CFLAGS)

clean:
	rm -rf obj bin
