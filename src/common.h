typedef struct Analysis_s
{
	int tot;
	int letters;
	int numbers;
	int spaces;
	int punctuaction;
	int other;

} Analysis;

#define FILENAME_Q "./Q.out"
#define FILENAME_P "./P.out"
#define FILENAME_C "./C.out"
#define FILENAME_A "./A.out"
#define FILENAME_R "./R.out"
#define FILENAME_M "./M.out"

#define COMMAND_FILE 'F'
#define COMMAND_NR_SECTION 'M'
#define COMMAND_SECTION 'N'
#define COMMAND_QUIT 'Q'

#define MAX_FILENAME_LENGHT 1024

#define ERR_CO_OUTOFRANGE 1


void errorKillAll(const int errCode);
void error(char msg[]);
void loggN(int n);
void logg(char msg[]);
int readline(const int file, char *buffer, const int maxsize);
char readFirstChar(const int file);
int readNumber(const int file);
Analysis initAnalysis();
void printAnalysis(Analysis a);
Analysis readAnalysis(const int file);
Analysis sumAnalysis(Analysis a, Analysis b);
void sendCommand(const int file, char *cmd);
void sendIntCommand(const int file, const int cmd);
void sendCharCommand(const int file, const char cmd);


