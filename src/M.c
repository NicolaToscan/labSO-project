#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

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
void ll(){
    printf("\n");
}

int main(int argc, char *argv[])
{
    char *files[MAXNFILES];
    int nfile = 0, p, q;


    if(argc == 1)
    {

    }else
    {
        int i;
        for ( i = 0; i < argc-1; i++)     // tutti i file passati 
        {    
            files[i] = argv[i+1];
            nfile = argc-1;
        }
        
    }
    printf("mi hai dato %d file", nfile);
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
    int scelta;
    int quit = 1;
    printf("ecco la lista delle azioni che puoi fare:\n");
    printf("1) aggiungi un file \n");
    printf("2) visualizza file \n");
    printf("3) modifica p\n");
    printf("4) modifica q\n");
    printf("5) calcola report \n");
    printf("6) chiudi\n");
    printf("inserire il numero dell'operazione desiderata : ");
    scanf("%d", &scelta );

    switch (scelta)
    {
        case 1: addFiles(files, nfile) ; break;
        case 2: viewFiles(files, nfile); break;
        case 3: askp(p); break;
        case 4: askq(q); break;
        case 5: doReport(files, *nfile) ; break;
        case 6: quit = 0; break;
        default: printf("errore num errato");
    }       
    printf(" !!!il file è %s \n", files[*nfile]); 
    return  quit;
}

// prendere file dall'utente 
void addFiles(char *files[], int *nfile)
{    
    char *file = malloc(100 * sizeof(char));
    printf(" nfile = %d \n", *nfile);
    printf("inserire il nome del file da aggiungere : ");
    scanf("%s", file);
    printf(" il file è %s \n", file);

    ++(*nfile);
    files[*nfile-1] = file;
    printf(" !!!il file è %s \n", files[*nfile]);  

   // strcpy(files[*nfile], file);
    //strcpy(files[++(*nfile)], file);
    
   // files[(*nfile)] = "pipponw";
   //strcpy(files[*nfile-1], file);
    //strcpy(files[a], file);
    
}


void viewFiles(char **files, int *nfile)
{
    printf(" View il file è %s \n", files[*nfile]);
    int i ;
    printf("il numero di file è %d \n", *nfile);
    for (i = 0; i < *nfile; i++)
    {
        printf("%s \n", files[i]);  
    }
    ll();
    
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

