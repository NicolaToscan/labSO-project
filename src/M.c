#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "common.h"

// TODO aggiustare input da stdin perchè gets è pericolosa
// se in scelta metti un carattere scoppia tutto 
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


	if(argc == 1)
	{   
		char buffer[128];
		char *parole[10];
		printf("non hai inserito nessun file, per continuare perfavore aggiungine almeno uno: \n");
		readline(1,buffer, 128 );
		printf("%s \n", buffer);


	}else
	{
		int i;
		for ( i = 0; i < argc-1; i++)     // tutti i file passati 
		{    
			if(checkFile(argv[i+1])){
				files[i] = argv[i+1];
				nfile ++;
			}
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
}
int askq(int *q)
{
	printf("e il numero di pezzi in cui devo suddividere il file (q)?\n");
	scanf("%d", q);
}
int scelta(int *p, int*q, char **files, int *nfile)
{
	char *scelta;
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
	read(1, scelta, 1);
	printf("%c", *scelta);
		switch (*scelta)
		{
			case '1': addFiles(files, nfile) ; break;
			case '2': viewFiles(files, nfile); break; // non va 
			case '3': askp(p); break;
			case '4': askq(q); break;
			case '5': doReport(files, *nfile) ; break; // non va 
			case '6': quit = 0; break;
			default: error("errore num errato");
		}

		
	   
	return  quit;
}

// prendere file dall'utente 
void addFiles(char *files[], int *nfile)
{    
	char *file = malloc(100 * sizeof(char));
	printf("inserire il nome del file da aggiungere : ");
	scanf("%s", file);
	if(checkFile(file))
	{
		++(*nfile);
		files[*nfile-1] = file;
	}else{
		error(" file not found");
		free(file);
	}
	
}


void viewFiles(char **files, int *nfile)
{
	
	int i ;
	for (i = 0; i < *nfile; i++)
	{
		printf("%s \n", files[i]);  
	}
	ll();
	
}

int checkFile(char *file){
	return (open(file, O_RDONLY) < 0) ? 0:1;
}
//chiama analyzer e gli passa files
void doReport(char **files, int nfile)
{
	stampaReport();
}

void stampaReport()
{
	//capire come vogliamo formattare il report 
}


