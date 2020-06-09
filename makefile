# Comando: make 	= crea cartella /bin con eseguibili
# Comando: make -s	= uguale a make ma senza output
# Comando: make clean	= elimina /bin

CC = gcc
STANDARD = -std=gnu90

THREAD = -pthread

SRC_DIR = src
LIB_DIR = src/lib
BIN_DIR = bin

OUTS = $(BIN_DIR)/M.out $(BIN_DIR)/A.out $(BIN_DIR)/R.out $(BIN_DIR)/C.out $(BIN_DIR)/P.out $(BIN_DIR)/Q.out

LIBS_O = $(BIN_DIR)/analisys.o $(BIN_DIR)/commands.o $(BIN_DIR)/common.o $(BIN_DIR)/communication.o $(BIN_DIR)/filemanager.o
LIBS_C = $(LIB_DIR)/analisys.c $(LIB_DIR)/commands.c $(LIB_DIR)/common.c $(LIB_DIR)/communication.c $(LIB_DIR)/filemanager.c

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
	-rm bin/*.o

# Help
.PHONY: help
help:
