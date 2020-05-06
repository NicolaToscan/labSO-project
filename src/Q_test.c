#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "common.h"
#include "dataprotocol.h"

int main(int argc, char *argv[])
{

    logg("Ciaone da Q");
    char buff[4096];
    while (1)
    {
        readline(STDIN_FILENO, buff, 4096);
        logg(buff);
    }
    return 0;
}