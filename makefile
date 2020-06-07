# Comando: make 	= crea cartella /bin con eseguibili
# Comando: make -s	= uguale a make ma senza output
# Comando: make clean	= elimina /bin

# Easy version
project: bin/M bin/P bin/Q bin/A bin/C

bin/M: src/lib/filemanager.c src/lib/analisys.c src/lib/common.c src/lib/communication.c src/M.c
		mkdir -p bin
		gcc -std=gnu90 src/lib/filemanager.c src/lib/analisys.c src/lib/common.c src/lib/communication.c src/M.c -o bin/M.out
	   
bin/Q:  src/lib/filemanager.c src/lib/analisys.c src/lib/common.c src/lib/communication.c src/Q.c
		mkdir -p bin
		gcc -std=gnu90 src/lib/filemanager.c src/lib/analisys.c src/lib/common.c src/lib/communication.c src/Q.c -o bin/Q.out

bin/C:  src/lib/analisys.c src/lib/common.c src/lib/communication.c src/C.c
		mkdir -p bin
	    gcc -std=gnu90 src/lib/filemanager.c src/lib/analisys.c src/lib/common.c src/lib/communication.c src/C.c -o bin/C.out

bin/A: src/lib/filemanager.c src/lib/analisys.c src/lib/common.c src/lib/communication.c src/A.c
		mkdir -p bin
	   gcc  -pthread -std=gnu90 src/lib/filemanager.c src/lib/analisys.c src/lib/common.c src/lib/communication.c src/A.c -o bin/A.out

bin/P:  src/lib/filemanager.c src/lib/analisys.c src/lib/common.c src/lib/communication.c src/P.c
		mkdir -p bin
	    gcc -std=gnu90 src/lib/filemanager.c src/lib/analisys.c src/lib/common.c src/lib/communication.c src/P.c -o bin/P.out


# Clean
clean:
	-rm bin/*
	-rmdir bin

