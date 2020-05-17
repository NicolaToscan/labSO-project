#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "common.h"

#define COMMAND_ADD 'A'
#define COMMAND_SETP 'P'
#define COMMAND_SETQ 'Q' // potrebbe fare interferenza col quit  
#define COMMAND_VIEW 'V' 
#define COMMAND_KILL 'K' 
#define MAXNFILES 128

void addFile(char files[][MAX_FILENAME_LENGHT], int *nfile, char *filename);
int checkFile(char *file);
void view(char files[][MAX_FILENAME_LENGHT], int *nfile);


int main(){
    char fileName[MAX_FILENAME_LENGHT];
    char files[MAXNFILES][MAX_FILENAME_LENGHT];
	int nfile = 0, P,Q, quit = 1;
    
    
    while(quit)
    {
        char cmd = readFirstChar(STDIN_FILENO);
        switch (cmd)
        {
        case COMMAND_ADD:
            
            readline(STDIN_FILENO, fileName, MAX_FILENAME_LENGHT);
            addFile(files,&nfile, fileName);          
            break;
        case COMMAND_SETP:
            P = readNumber(STDIN_FILENO);
            
            break;
        case COMMAND_SETQ:
            Q = readNumber(STDIN_FILENO);        
            break;
        case COMMAND_VIEW: 
            view(files,&nfile);
            break;
        case COMMAND_KILL:
            quit = 0;
            break;
        default:
            break;
        }
    }
   
    logg("mi hai ucciso ");

    return 0;

}

void view(char files[][MAX_FILENAME_LENGHT], int *nfile){
    int i;

    loggN(*nfile);
    for ( i = 0; i < *nfile; i++)
    {
        sendCommand(STDOUT_FILENO, files[i]);
    }

   

}


void addFile(char files[][MAX_FILENAME_LENGHT], int *nfile, char *filename)
{    

	if(checkFile(filename))
	{
        strcpy(files[*nfile], filename);
		++(*nfile);
        sendIntCommand(STDOUT_FILENO, 1);
        //write(STDOUT_FILENO, files[i], MAX_FILENAME_LENGHT)
		
		logg("file aggiunto da A");
		
	}else{
		error(" file not found");
        sendIntCommand(STDOUT_FILENO, 0);
	}
	
	
}

int checkFile(char *file){
	int nfile = open(file, O_RDONLY);
	close(nfile);
	return (nfile < 0) ? 0:1;
}