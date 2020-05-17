#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "common.h"

#define MAXNFILES 128
#define READ 0
#define WRITE 1


void doReport( int nfile);
void stampaReport();
void viewFiles(int *nfiles);
void addFiles( int *nfiles);
int scelta(int *p, int *q, int *nfile);
int askp(int *p);
int askq(int *q);

void ll(){printf("\n");}
void startA();

int fdMtoA[2];
int fdAtoM[2];

int main(int argc, char *argv[])
{
	
	int nfile = 0, p, q;
	startA();

	int i; // manda gli argomenti ad A
	for ( i = 1; i < argc; i++)     
	{    		
		sendCharCommand(fdMtoA[WRITE], 'A');
		sendCommand(fdMtoA[WRITE], argv[i]); 	
		nfile += readNumber(fdAtoM[READ]); 		
	}
	printf("mi hai dato %d file validi ", nfile);

	askp(&p);
	askq(&q);
	
	while(scelta(&p, &q,  &nfile));	 
	return 0;
}

int askp(int *p)
{    
	ll();
	printf("ora dimmi il numero di gruppi di file (p) \n");
	*p = readNumber(STDIN_FILENO);
	sendCharCommand(fdMtoA[WRITE], 'P');
	sendIntCommand(fdMtoA[WRITE], *p);
	printf("p adesso vale %d\n", *p );
}
int askq(int *q)
{
	ll();
	printf("dimmi il numero di pezzi in cui devo suddividere il file (q)?\n");
	*q = readNumber(STDIN_FILENO);
	sendCharCommand(fdMtoA[WRITE], 'Q');
	sendIntCommand(fdMtoA[WRITE], *q);
	printf("q adesso vale %d\n", *q);
}
int scelta(int *p, int *q, int *nfile)
{
	char scelta;
	int quit = 1;
	ll();
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
		case ADD_FILES: addFiles(nfile) ; break;
		case VIEW_FILES: viewFiles( nfile); break; 
		case EDIT_P: askp(p); break;
		case EDIT_Q: askq(q); break;
		case REPORT: doReport( *nfile) ; break;
		case QUIT: quit = 0;sendCharCommand(fdMtoA[WRITE], 'K'); break;
		default: error("errore num errato");
	}

	return  quit;
}

// prendere file dall'utente 
void addFiles( int *nfiles)
{    
	// prende in input dall'utente
	char file[MAX_FILENAME_LENGHT];
	printf("inserire il nome del file da aggiungere : \n");
	readline(STDIN_FILENO, file, MAX_FILENAME_LENGHT);
	
	//invia ad A
	sendCharCommand(fdMtoA[WRITE], 'A');
	sendCommand(fdMtoA[WRITE], file); 
	*nfiles += readNumber(fdAtoM[READ]); 	
}


void viewFiles(int *nfile)
{	
	char file[MAX_FILENAME_LENGHT];
	//chiede e riceve i nomi dei file 
	sendCharCommand(fdMtoA[WRITE], 'V');
	int i;
	for ( i = 0; i < *nfile; i++)
	{
		readline(fdAtoM[READ], file, MAX_FILENAME_LENGHT);
		logg(file);	
	}
}



void doReport(int nfile)
{
	

	stampaReport();
}

void stampaReport()
{
	//capire come vogliamo formattare il report 
}

// inizializza pipes
void startA(){
	pipe(fdMtoA);
	pipe(fdAtoM);
 
	pid_t pid = fork();
	if(pid > 0 ) // parent
	{
		close(fdMtoA[READ]);
		close(fdAtoM[WRITE]);

	}else if(pid == 0)// child
	{ 
		close(fdMtoA[WRITE]) ;
		close(fdAtoM[READ]);

		dup2(fdMtoA[READ], STDIN_FILENO); // adesso il figlio ha come standard input l'estremità della pipe
		dup2(fdAtoM[WRITE], STDOUT_FILENO); // il figlio ha come standard output M

		if(execlp(FILENAME_A, FILENAME_A, (char *)NULL) < 0)
		{
			error("EXEC error");
		}
	}else
	{
		error("FORK error");
	}
}

