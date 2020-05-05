typedef struct Analysis_s
{
	int tot;
	int letters;
	int numbers;
	int spaces;
	int punctuaction;
	int other;

} Analysis;

int readline(int file, char *buffer, int maxsize);