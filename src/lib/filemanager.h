
typedef struct fileAccessGuide
{
    char path[MAX_PATH_LENGHT];
    int totParts;
    int myPart;
} FileGuide;

void splitFiles(char *file, int parts, int *indexes);

pair getFileRange(uint32 fileLength, int part, int totParts);
pair getFilesRanges(uint32 fileLength, int part, int totParts);
