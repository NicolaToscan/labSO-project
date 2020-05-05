typedef struct Analysis_s
{
	int tot;
	int letters;
	int numbers;
	int spaces;
	int punctuaction;
	int other;

} Analysis;


#define COMMAND_FILE 'F'
#define COMMAND_NR_SECTION 'M'
#define COMMAND_SECTION 'N'
#define COMMAND_QUIT 'Q'

#define MAX_FILENAME_LENGHT 1024


void error(char msg[]);
int readline(const int file, char *buffer, const int maxsize);
char readFirstChar(const int file);
int readNumber(int file);
Analysis initAnalysis();
void printAnalysis(Analysis *a);
Analysis readAnalysis(int file);
