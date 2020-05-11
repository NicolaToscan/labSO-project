#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "common.h"

#define MAXNFILES 128


void doReport(char **files, int nfile);
void stampaReport();
void viewFiles(char ** files, int *nfiles);
void addFiles(char *files[], int *nfile);
int scelta(int *p, int *q, char **files, int *nfile);
int askp(int *p);
int askq(int *q);

void ll(){printf("\n");}
void startA();

int fd[2];

int main(int argc, char *argv[])
{
	char *files[MAXNFILES];
	int nfile = 0, p, q;
	startA();
	int i;
	for ( i = 0; i < argc-1; i++)     
	{    
		 	files[i] = argv[i+1];
			nfile ++;
			sendCharCommand(fd[1], 'A');
			sendCommand(fd[1], files[i]); 	 
	}
		
	
	printf("mi hai dato %d file validi ", nfile);
	askp(&p);
	askq(&q);
	
	while(scelta(&p, &q, files, &nfile));	 
	return 0;

}
int askp(int *p)
{    
	printf("ora dimmi il numero di gruppi di file (p) \n");
	*p = readNumber(STDIN_FILENO);
	sendCharCommand(fd[1], 'P');
	sendIntCommand(fd[1], *p);
	printf("p adesso vale %d\n", *p );
}
int askq(int *q)
{
	printf("dimmi il numero di pezzi in cui devo suddividere il file (q)?\n");
	*q = readNumber(STDIN_FILENO);
	sendCharCommand(fd[1], 'Q');
	sendIntCommand(fd[1], *q);
	printf("q adesso vale %d\n", *q);
}
int scelta(int *p, int *q, char **files, int *nfile)
{
	char scelta;
	int quit = 1;
	printf("ecco la lista delle azioni che puoi fare:\n");
	printf("1) aggiungi un file \n");
	printf("2) visualizza file \n");
	printf("3) modifica p\n");
	printf("4) modifica q\n");
	printf("5) calcola report \n");
	printf("6) chiudi\n");
	printf("inserire il numero dell'operazione desiderata : ");
	ll();
	scelta = readFirstChar(STDIN_FILENO);
	

	switch (scelta)
	{
		case ADD_FILES: addFiles(files, nfile) ; break;
		case VIEW_FILES: viewFiles(files, nfile); break; 
		case EDIT_P: askp(p); break;
		case EDIT_Q: askq(q); break;
		case REPORT: doReport(files, *nfile) ; break;
		case QUIT: quit = 0; break;
		default: error("errore num errato");
	}

	return  quit;
}

// prendere file dall'utente 
void addFiles(char *files[], int *nfile)
{    
	char file[MAX_FILENAME_LENGHT];
	printf("inserire il nome del file da aggiungere : \n");
	readline(STDIN_FILENO, file, MAX_FILENAME_LENGHT);


	++(*nfile);
	files[*nfile-1] = file;
	logg("file aggiunto");
	sendCharCommand(fd[1], 'A');
	sendCommand(fd[1], file); // invia il file ad A

	
	
}


void viewFiles(char **files, int *nfile)
{
	if( *nfile>0){
		int i ;
		for (i = 0; i < *nfile; i++)
		{
			printf("%s \n", files[i]);  
		}
		ll();
	}else{
		printf("nessun file è stato aggiunto\n");
	}
	
}



void doReport(char **files, int nfile)
{
	

	stampaReport();
}

void stampaReport()
{
	//capire come vogliamo formattare il report 
}

void startA(){
	const int WRITE = 1;
	const int READ = 0;

	pipe(fd);
	
	pid_t pid = fork();
	if(pid > 0 ) // parent
	{
		close(fd[READ]);
		
		
	}else if(pid == 0){ // child

		close(fd[WRITE]) ;
		dup2(fd[0], STDIN_FILENO); // adesso il figlio ha come standard input l'estremità della pipe
		if(execlp(FILENAME_A, FILENAME_A, (char *)NULL) < 0){
			error("EXEC error");
		}
	}else{
		error("FORK error");
	}
}

