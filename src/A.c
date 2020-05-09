#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "common.h"


int main(){
    char parola[50];
    readline(STDIN_FILENO, parola, 50);
    logg("eccomi");
    printf("parola = %s\n", parola);
    return 0;
}