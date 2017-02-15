#include <stdlib.h>
#include <string.h>

int line;

int convert_to_binary(int num);

void incrementer(char *incoming, char *outgoing)
{
	line = (int)strtol(incoming, 0, 2);
	line++;
	line = convert_to_binary(line);
	sprintf(outgoing, "%d", line);
}

int convert_to_binary(int num)
{
	int dec;
	int remain;
	int base = 1;
	int binary = 0;

	dec = num;

	while (num > 0)
	{
		remain = num % 2;
		binary = binary + remain * base;
		num = num / 2;
		base = base * 10;
	}
	return binary;
}