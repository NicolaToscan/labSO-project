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
void addFiles( int *nfiles);
int scelta(int *p, int *q, char **files, int *nfile);
int askp(int *p);
int askq(int *q);

void ll(){printf("\n");}
void startA();

int fdMtoA[2];
int fdAtoM[2];

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
			sendCharCommand(fdMtoA[1], 'A');
			sendCommand(fdMtoA[1], files[i]); 	 
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
	sendCharCommand(fdMtoA[1], 'P');
	sendIntCommand(fdMtoA[1], *p);
	printf("p adesso vale %d\n", *p );
}
int askq(int *q)
{
	printf("dimmi il numero di pezzi in cui devo suddividere il file (q)?\n");
	*q = readNumber(STDIN_FILENO);
	sendCharCommand(fdMtoA[1], 'Q');
	sendIntCommand(fdMtoA[1], *q);
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
		case ADD_FILES: addFiles(nfile) ; break;
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
void addFiles( int *nfiles)
{    
	char file[MAX_FILENAME_LENGHT];
	printf("inserire il nome del file da aggiungere : \n");
	readline(STDIN_FILENO, file, MAX_FILENAME_LENGHT);

	//invia ad A
	sendCharCommand(fdMtoA[1], 'A');
	sendCommand(fdMtoA[1], file); 

	
	
}


void viewFiles(char **files, int *nfile)
{
	/*char file[MAX_FILENAME_LENGHT];
	sendCharCommand(fdMtoA[1], 'V');
	int i;
	for ( i = 0; i < *nfile; i++)
	{
		readline(fdAtoM[0], file, MAX_FILENAME_LENGHT);
		logg(file);
		logg("M");
	}*/
	
		
	
	
	
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
 const int WRITE = 1;   // non è meglio un define?
 const int READ = 0;

 pipe(fdMtoA);
 pipe(fdAtoM);
 
 pid_t pid = fork();
 if(pid > 0 ) // parent
 {
  close(fdMtoA[READ]);
  close(fdAtoM[WRITE]);

   
 }else if(pid == 0){ // child

  close(fdMtoA[WRITE]) ;
  close(fdAtoM[READ]);

  dup2(fdMtoA[READ], STDIN_FILENO); // adesso il figlio ha come standard input l'estremità della pipe
  dup2(fdAtoM[WRITE], STDOUT_FILENO); // il figlio ha come standard output M

  if(execlp(FILENAME_A, FILENAME_A, (char *)NULL) < 0){
   error("EXEC error");
  }
 }else{
  error("FORK error");
 }
}

