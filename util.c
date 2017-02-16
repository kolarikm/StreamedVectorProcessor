/*********************************************************
* Streamed Vector Processor Utility
* CIS 452 Winter 2017
* Author: Michael Kolarik
*
* Contains helper functions for the main streamed vector 
* processor to execute.
*********************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//Used to store results of number operations
int inc_res, comp_res, add_a_res, add_b_res, add_res;

//Function definitions
void complement_number(char *incoming, char *outgoing, int len);
void increment_number(char *incoming, char *outgoing, int len);
int convert_to_binary(int num);
void add_numbers(char *add_a, char *add_b, char *outgoing, int len);
void setup_pipes(int *fd_one, int *fd_two);

/*********************************************************
* Receives pointers to two separate character arrays -
* the first of which is the read value, with the second
* being the destination for the modified values. The
* length of the bit strings used to determine array end
*********************************************************/
void complement_number(char *incoming, char *outgoing, int len)
{
	incoming[len] = '\0';
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

/*********************************************************
* First, convert the incoming binary string into an
* integer, then add one to the value, and re-encode as a
* binary string before copying to array. Length used to
* determine proper formatting
*********************************************************/
void increment_number(char *incoming, char *outgoing, int len)
{
	inc_res = (int)strtol(incoming, 0, 2);
	inc_res++;
	inc_res = convert_to_binary(inc_res);
	//Convert result to int and pad with len number of 0s
	sprintf(outgoing, "%0*d", len, inc_res);
}

/*********************************************************
* Takes an integer value and returns its binary
* equivalent to the calling function
*********************************************************/
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

/*********************************************************
* Performs an addition on two bitstrings by converting
* each to a number, adding, and then converting back to
* a binary representation of the number
*********************************************************/
void add_numbers(char *add_a, char *add_b, char *outgoing, int len)
{
	add_a_res = (int)strtol(add_a, 0, 2);
	add_b_res = (int)strtol(add_b, 0, 2);
	add_res = add_a_res + add_b_res;
	add_res = convert_to_binary(add_res);
	//Convert result to int and pad with len number of 0s
	sprintf(outgoing, "%0*d", len, add_res);
}

/*********************************************************
* Take two integer array pointers which will be filled
* with file descriptors to be used by the pipes
*********************************************************/
void setup_pipes(int *fd_one, int *fd_two)
{
	//Create two pipes for passing data between child processes
	if (pipe(fd_one) < 0)
	{
		perror("Failed to create pipe 1");
		exit(1);
	}
	if (pipe(fd_two) < 0)
	{
		perror("Failed to create pipe 2");
		exit(1);
	}
}