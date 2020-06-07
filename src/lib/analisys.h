#define AN_UPPLT 0
#define AN_LOWLT 1
#define AN_NUMBR 2
#define AN_MATHS 3
#define AN_PUNCT 4
#define AN_BRCKT 5
#define AN_SPACE 6
#define AN_OTEXT 7
#define AN_OTHER 8

#define ANAL_LENGTH 9

typedef struct Analysis_s
{
	unsigned long values[9];
} Analysis;

Analysis initAnalysis();

Analysis analyseFile(char *fileName, int mySection, int totSections);

void printAnalysis(const int file, Analysis a);
Analysis readAnalysis(const int file);

void addCharAnalysis(Analysis *a, char c);

int isText(char c);
int isUppLetter(char c);
int isLowLetter(char c);
int isNumber(char c);
int isMathSymbol(char c);
int isPunctuation(char c);
int isBracket(char c);
int isSpace(char c);
void printAnalysisReadable(Analysis a);
void sumAnalysis(Analysis *res, Analysis a);
