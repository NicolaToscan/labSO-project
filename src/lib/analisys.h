typedef struct Analysis_s
{
	int val;
} Analysis;

Analysis initAnalysis();
void printAnalysis(Analysis a);
Analysis readAnalysis(const int file);
