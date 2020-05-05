typedef struct Analysis_s
{
	int tot;
	int letters;
	int numbers;
	int spaces;
	int punctuaction;
	int other;

} Analysis;


#define Q_FILE_COMMAND 'F'
#define Q_NUMBER_OF_SECTIONS_COMMAND 'M'
#define Q_MY_SECTION_COMMAND 'N'
#define Q_QUIT_COMMAND 'Q'

#define MAX_FILENAME_LENGHT 1024


int readline(const int file, char *buffer, const int maxsize);
char readFirstChar(const int file);
void error(char msg[]);
int readNumber(int file);
Analysis initAnalysis();
void printAnalysis(Analysis *a);
Analysis readAnalysis(int file);
