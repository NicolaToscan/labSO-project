#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MAXFILESIZE 1024

typedef struct Analysis_s
{
	int tot;
	int letters;
	int numbers;
	int spaces;
	int punctuaction;
	int other;

} Analysis;

Analysis newAnalysis();
void analyseChar(char c, Analysis *a);
void printAnalysis(Analysis *a);

int main(int argc, char *argv[])
{
	if (argc == 1)
	{
		printf("Usare: Q <section-to-read> <number-of-sections>\n\n");
		exit(0);
	}

	if (argc != 3)
	{
		fprintf(stderr, "Error: 2 parameters where expected, but %d where found\n\n", argc);
		exit(1);
	}

	Analysis analysis = newAnalysis();

	char filename[MAXFILESIZE];
	while (1)
	{
		fgets(filename, MAXFILESIZE, stdin);
		int len = strlen(filename);
		if (filename[len - 1] == '\n')
			filename[len - 1] = '\0';
		filename[MAXFILESIZE - 1] = '\0';
		FILE *fp;
		fp = fopen(filename, "r");
		if (fp <= 0)
		{
			fprintf(stderr, "Error reading file\n");
			printf("N\n");
		}
		else
		{
			while (!feof(fp))
			{
				int c = fgetc(fp);
				analyseChar(c, &analysis);
			}
			printf("Y\n");
			printAnalysis(&analysis);
		}
	}

	return 0;
}

Analysis newAnalysis()
{
	Analysis analysis;
	analysis.tot = 0;
	analysis.letters = 0;
	analysis.numbers = 0;
	analysis.spaces = 0;
	analysis.punctuaction = 0;
	analysis.other = 0;
	return analysis;
}

void analyseChar(char c, Analysis *a)
{
	a->tot++;
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		a->letters++;
	else if ((c >= '1' && c <= '9'))
		a->numbers++;
	else if (c == ' ')
		a->spaces++;
	else
		a->other++;
}

void printAnalysis(Analysis *a)
{
	printf("%d, %d, %d, %d, %d, %d\n", a->tot, a->letters, a->numbers, a->spaces, a->punctuaction, a->other);
}