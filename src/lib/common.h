
#define FILENAME_Q 'Q'
#define FILENAME_P 'P'
#define FILENAME_C 'C'
#define FILENAME_A 'A'
#define FILENAME_R 'R'
#define FILENAME_M 'M'

#define NAMED_PIPE "/tmp/myfifo"

#define MAX_PATH_LENGHT 4096
#define MAX_CMD_LENGHT 8192

#define WRITE 1
#define READ 0

#define DEFAULT_P 3
#define DEFAULT_Q 4

// ERRORS
#define ERR_FORK 1
#define ERR_EXEC 2
#define ERR_PIPE 3
#define ERR_INVALID_NR 4


typedef enum
{
    false,
    true
} bool;

typedef struct pair_p
{
    int first;
    int second;
} pair;

void error(char msg[]);
void logg(char msg[]);
void loggN(int n);
void loggC(char c);

void clearLine(const int file);
char readchar(const int file);
int readline(const int file, char *buffer, const int maxsize);

void execErrorHandleAndExit(int out, int pipeToCloseA, int pipeToCloseB);
void forkErrorHandle(int pA, int pB, int pC, int pD);

void getExecFilename(char fileNameId, char *buff);