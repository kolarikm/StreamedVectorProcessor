/**************************
* Streamed Vector Processor
* CIS 452 Winter 2017
* Author: Michael Kolarik
**************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// Define constants and functions
void setup_pipes(int *fd_one, int *fd_two);

int main(int argc, char * argv[])
{
	//signal (SIGINT, handle_delay);

	// !!!! Write handle_command_line function

	//Command line argument checking and error reporting
	if (argc != 5)
	{
		fprintf(stderr, "Error initializing program\n");
		fprintf(stderr, "Usage: ./svp InputA, InputB, BitStringLength, NumberOfLines\n");
		exit(-1);
	}

	//Create file pointers for input files from argument names
	char *inputA = argv[1];
	char *inputB = argv[2];
	fprintf(stdout, "InputA is: %s\n", inputA);
	fprintf(stdout, "InputB is: %s\n", inputB);

	//Parse bitstring length and number of lines from arguments
	int line_length = atoi(argv[3]);
	int num_lines = atoi(argv[4]);
	fprintf(stdout, "BitStringLength is: %d\n", line_length);
	fprintf(stdout, "NumberOfLines is: %d\n", num_lines);

	//Create two file descriptors to be used by pipes
	//These can be moved to global space
	int fd_one[2];
	int fd_two[2];

	//Set up the pipes
	setup_pipes(fd_one, fd_two);

	int i;
	pid_t pid;
	for (i = 0; i <= 1; i++)
	{
		if (pid = fork() < 0)
		{
			fprintf(stderr, "Unable to create child process %d\n", i+1);
			exit(-1);
		}
		else if (pid == 0)
		{
			fprintf(stdout, "Created child process %d, PID: %d\n", getpid());

			switch (i)
			{
				case 0:
				{
					//Executing the first child process, the incrementer
				}

				case 1:
				{
					//Executing the second child process, the adder
				}
				
			}			
		}
		exit(0);
	}
}

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