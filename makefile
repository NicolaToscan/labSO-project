CC = gcc
STANDARD = -std=gnu90

THREAD = -pthread

SRC_DIR = src
LIB_DIR = src/lib
BIN_DIR = bin

PRJCTS = M A R C P Q
LIBS = analisys commands common communication filemanager

####

LIBS_C = $(addprefix $(LIB_DIR)/, $(addsuffix .c, $(LIBS)))
LIBS_O = $(addprefix $(BIN_DIR)/, $(addsuffix .o, $(LIBS)))

PRJCTS_C = $(addprefix $(SRC_DIR)/, $(addsuffix .c, $(PRJCTS)))
PRJCTS_O = $(addprefix $(BIN_DIR)/, $(addsuffix .o, $(PRJCTS)))

OUTS = $(addprefix $(BIN_DIR)/, $(addsuffix .out, $(PRJCTS)))

####

all: $(OUTS)

####

bin/M.out: bin/M.o $(LIBS_O)
	$(CC) $(STANDARD) $(THREAD) $^ -o $@

bin/A.out: bin/A.o $(LIBS_O)
	$(CC) $(STANDARD) $(THREAD) $^ -o $@

bin/R.out: bin/R.o $(LIBS_O)
	$(CC) $(STANDARD) $(THREAD) $^ -o $@

bin/C.out: bin/C.o $(LIBS_O)
	$(CC) $(STANDARD) $(THREAD) $^ -o $@

bin/P.out: bin/P.o $(LIBS_O)
	$(CC) $(STANDARD) $^ -o $@

bin/Q.out: bin/Q.o $(LIBS_O)
	$(CC) $(STANDARD) $^ -o $@


####

bin/M.o: src/M.c
	@mkdir -p bin
	$(CC) $(STANDARD) -c $^ -o $@

bin/A.o: src/A.c
	@mkdir -p bin
	$(CC) $(STANDARD) -c $^ -o $@

bin/R.o: src/R.c
	@mkdir -p bin
	$(CC) $(STANDARD) -c $^ -o $@

bin/C.o: src/C.c
	@mkdir -p bin
	$(CC) $(STANDARD) -c $^ -o $@

bin/P.o: src/P.c
	@mkdir -p bin
	$(CC) $(STANDARD) -c $^ -o $@

bin/Q.o: src/Q.c
	@mkdir -p bin
	$(CC) $(STANDARD) -c $^ -o $@

####

bin/analisys.o: src/lib/analisys.c
	@mkdir -p bin
	$(CC) $(STANDARD) -c $^ -o $@

bin/commands.o: src/lib/commands.h
	@mkdir -p bin
	$(CC) $(STANDARD) -c -x c $^ -o $@

bin/common.o: src/lib/common.c
	@mkdir -p bin
	$(CC) $(STANDARD) -c $^ -o $@

bin/communication.o: src/lib/communication.c
	@mkdir -p bin
	$(CC) $(STANDARD) -c $^ -o $@

bin/filemanager.o: src/lib/filemanager.c
	@mkdir -p bin
	$(CC) $(STANDARD) -c $^ -o $@

####

# Clean
.PHONY: clean
clean:
	@-rm bin/*.o
	@echo Deleted object files in bin/

# Force
.PHONY: force
force:
	@-rm bin/*.o
	@-rm bin/*.out
	@make

# Help
.PHONY: help
help:
	@echo Use make to create makefile
	@echo Use make -s to create makefile hiding commands
	@echo Use make clean to delete object files
	@echo Use make force to re-build everything


