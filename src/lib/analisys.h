#include "common.h"

typedef unsigned long ulong;
#define ULONG_MAXLEN 7

typedef struct Analysis_s
{
	ulong values[9];

	//

	ulong uppLetters() 		{ return values[0]; }
	ulong lowLetters() 		{ return values[1]; }

	ulong numbers() 		{ return values[2]; }
	ulong mathSymbols() 	{ return values[3]; }

	ulong punctuaction()	{ return values[4]; }
	ulong brackets() 		{ return values[5]; }
	ulong spaces() 			{ return values[6]; }

	ulong otherText() 		{ return values[7]; }
	ulong other() 			{ return values[8]; }
} Analysis;

Analysis initAnalysis();

void printAnalysis(const int file, Analysis a);
Analysis readAnalysis(const int file);

void intToStr(ulong value, char *str, int startIndex);
ulong strToInt(char *str, int startIndex);

void intToStr(ulong value, char str[ULONG_MAXLEN]); // Forse inutile
ulong strToInt(char str[ULONG_MAXLEN]); // Forse inutile

bool isText(char c);
bool isUppLetter(char c);
bool isLowLetter(char c);
bool isNumber(char c);
bool isMathSymbol(char c);
bool isPunctuation(char c);
bool isBracket(char c);
bool isSpace(char c);