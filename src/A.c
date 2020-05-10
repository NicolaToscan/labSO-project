#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "common.h"


int main(){
    char cmd[50];
    logg("eccomi");
    
    while(1)
    {
        readline(STDIN_FILENO, cmd, 50);
        logg(cmd);
    }
   
    

    return 0;

}