
#define FILENAME_Q "./bin/Q.out"
#define FILENAME_P "./bin/P.out"
#define FILENAME_C "./bin/C.out"
#define FILENAME_A "./bin/A.out"
#define FILENAME_R "./bin/R.out"
#define FILENAME_M "./bin/M.out"

#define MAX_PATH_LENGHT 4096
#define MAX_CMD_LENGHT 8192

#define WRITE 1
#define READ 0

// ERRORS
#define ERR_FORK 1
#define ERR_EXEC 2
#define ERR_INVALID_NR 3

typedef unsigned long uint32;
typedef unsigned long long uint64;

typedef enum
{
    false,
    true
} bool;

typedef struct pair_p
{
    uint32 first;
    uint32 second;
} pair;

void error(char msg[]);
void logg(char msg[]);
void loggN(int n);
void loggC(char c);

void sendCommand(const int file, char *cmd);
void sendIntCommand(const int file, const int cmd);
void sendCharCommand(const int file, const char cmd);

void clearLine(const int file);
char readchar(const int file);
int readline(const int file, char *buffer, const int maxsize);

void execErrorHandleAndExit(int out, int pipeToCloseA, int pipeToCloseB);
void forkErrorHandle(int pA, int pB, int pC, int pD);
