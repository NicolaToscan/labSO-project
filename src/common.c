int readline(int file, char *buffer, int maxsize)
{
	int i = 0;
	while (i < maxsize)
	{
		char CH[1];

		int rd = read(file, CH, 1);
		buffer[i] = CH[0];
		if (CH[0] == '\n')
		{
			buffer[i] = '\0';
			return i;
		}
		i++;
	}
	return -1;
}