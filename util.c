/******************************************
* Streamed Vector Processor Utility
* CIS 452 Winter 2017
* Author: Michael Kolarik
*
* Contains helper functions for the main
* streamed vector processor to execute
******************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//Used to store results of number operations
int inc_res;
int comp_res;

void complement_number(char *incoming, char *outgoing, int len);
void increment_number(char *incoming, char *outgoing);
int convert_to_binary(int num);

/******************************************
* 
******************************************/
void complement_number(char *incoming, char *outgoing, int len)
{
	int i;
	for (i = 0; i < len+1; i++)
	{
		if (incoming[i] == '1')
		{
			outgoing[i] = '0';
		}
		else
		{
		 	outgoing[i] = '1';
		 }
	}
	outgoing[len] = '\0';	
}

/******************************************
* First, convert the binary string into an
* integer, then add one, re-encode as a
* binary string before copying to array
******************************************/
void increment_number(char *incoming, char *outgoing)
{
	inc_res = (int)strtol(incoming, 0, 2);
	inc_res++;
	inc_res = convert_to_binary(inc_res);
	sprintf(outgoing, "%d", inc_res);
}

/******************************************
* 
******************************************/
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