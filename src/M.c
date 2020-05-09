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
int checkFile(char *file);
void ll(){
	printf("\n");
}


int main(int argc, char *argv[])
{
	char *files[MAXNFILES];
	int nfile = 0, p, q;

	int i;
	for ( i = 0; i < argc-1; i++)     
	{    
		if(checkFile(argv[i+1])){
			files[i] = argv[i+1];
			nfile ++;
		}
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
	scanf("%d", p);
	printf("p adesso vale %d\n", *p );
}
int askq(int *q)
{
	printf("dimmi il numero di pezzi in cui devo suddividere il file (q)?\n");
	scanf("%d", q);
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
			case VIEW_FILES: viewFiles(files, nfile); break; // non va 
			case EDIT_P: askp(p); break;
			case EDIT_Q: askq(q); break;
			case REPORT: doReport(files, *nfile) ; break; // non va 
			case QUIT: quit = 0; break;
			default: error("errore num errato");
		}

		
	   
	return  quit;
}

// prendere file dall'utente 
void addFiles(char *files[], int *nfile)
{    
	char file[MAX_FILENAME_LENGHT];
	printf("inserire il nome del file da aggiungere : ");
	scanf("%s", file);

	if(checkFile(file))
	{
		++(*nfile);
		files[*nfile-1] = file;
		logg("file aggiunto ");
	}else{
		error(" file not found");
	}
	
	
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

int checkFile(char *file){
	return (open(file, O_RDONLY) < 0) ? 0:1;
}
//chiama analyzer e gli passa files
void doReport(char **files, int nfile)
{
	const int WRITE = 1;
	const int READ = 0;
	int fd[2];
	pipe(fd);

	pid_t pid = fork();
	if(pid > 0 ) // parent
	{
		close(fd[READ]);
	}else if(pid == 0){ // child
		close(fd[WRITE]) ;

		if(execlp(FILENAME_A, FILENAME_A, (char *)NULL) < 0){
			error("EXEC error");
		}
	}else{
		error("FORK error");
	}

	sendCommand(FILENAME_A, "ciao pipe");

	stampaReport();
}

void stampaReport()
{
	//capire come vogliamo formattare il report 
}


