# Comando: make 	= crea cartella /bin con eseguibili
# Comando: make -s	= uguale a make ma senza output
# Comando: make clean	= elimina /bin

# Easy version
project: bin/M bin/P bin/Q bin/A

bin/M: src/M.c src/common.c
	mkdir -p bin
	gcc -std=gnu90 -o bin/M src/M.c src/common.c -I.

bin/P: src/P.c src/common.c src/dataprotocol.c
	mkdir -p bin
	gcc -std=gnu90 -o bin/P src/P.c src/common.c src/dataprotocol.c -I.

bin/Q: src/Q.c src/common.c src/dataprotocol.c
	mkdir -p bin
	gcc -std=gnu90 -o bin/Q src/Q.c src/common.c src/dataprotocol.c -I.

bin/A: src/A.c src/common.c
	mkdir -p bin
	gcc -std=gnu90 -o bin/A src/A.c src/common.c -I.

# Clean
clean:
	-rm bin/*
	-rmdir bin