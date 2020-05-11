#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "common.h"

#define COMMAND_ADD 'A'
#define COMMAND_SETP 'P'
#define COMMAND_SETQ 'Q' // potrebbe fare interferenza col quit  
#define MAXNFILES 128

void addFile(char *files[], int *nfile, char *filename);
int checkFile(char *file);

int main(){
    char fileName[MAX_FILENAME_LENGHT];
    int P ;
    int Q;

    char *files[MAXNFILES];
	int nfile = 0;
    
    while(1)
    {
        char cmd = readFirstChar(STDIN_FILENO);
        
        switch (cmd)
        {
        case COMMAND_ADD:
            readline(STDIN_FILENO, fileName, MAX_FILENAME_LENGHT);
            addFile(files, &nfile , fileName);
            break;
        case COMMAND_SETP:
            P = readNumber(STDIN_FILENO);
            loggN(P);
            break;
        case COMMAND_SETQ:
            Q = readNumber(STDIN_FILENO);
            loggN(Q);
            break;
        
        default:
            break;
        }
    }
   
    

    return 0;

}


void addFile(char *files[], int *nfile, char *filename)
{    

	if(checkFile(filename))
	{
		++(*nfile);
		files[*nfile-1] = filename;
		logg("file aggiunto da A");
		
	}else{
		error(" file not found");
	}
	
	
}

int checkFile(char *file){
	int nfile = open(file, O_RDONLY);
	close(nfile);
	return (nfile < 0) ? 0:1;
}