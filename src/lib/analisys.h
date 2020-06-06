typedef unsigned long ulong;
#define ULONG_MAXLEN 7

#define DIVCHAR '#'

typedef struct Analysis_s
{
	ulong uppLetters;
	ulong lowLetters;

	ulong numbers;
	ulong mathSymbols;

	ulong punctuaction;
	ulong brackets;
	ulong spaces;

	ulong otherText;
	ulong other;
} Analysis;

Analysis initAnalysis();
void intToStr(ulong value, char str[ULONG_MAXLEN]);
void strToInt(char str[ULONG_MAXLEN], ulong *value);

void printAnalysis(const int file, Analysis a);
Analysis readAnalysis(const int file);

bool isText(char c);
bool isUppLetter(char c);
bool isLowLetter(char c);
bool isNumber(char c);
bool isMathSymbol(char c);
bool isPunctuation(char c);
bool isBracket(char c);
bool isSpace(char c);