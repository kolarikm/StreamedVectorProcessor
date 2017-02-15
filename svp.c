/******************************************
* Streamed Vector Processor
* CIS 452 Winter 2017
* Author: Michael Kolarik
*
*
*
******************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "util.c"

//File descriptors to be used by pipes
int fd_one[2];
int fd_two[2];

//Used to force first process to wait for go ahead signal
int delayed = 1;

//Command line arguments will set these config variables
int line_length;
int num_lines;
char *input_a;
char *input_b;

//File pointers to open the input files and output file
FILE *file_in_1;
FILE *file_in_2;
FILE *file_out;

void setup_pipes(int *fd_one, int *fd_two);
void parse_command_line(int argc, char *argv[]);
void handle_delay(int num);


/******************************************
* 
******************************************/
int main(int argc, char *argv[])
{
	//Assign function to execute for interrupt signal
	signal(SIGINT, handle_delay);

	//Handle command line arguments
	parse_command_line(argc, argv);

	//Set up the pipes
	setup_pipes(fd_one, fd_two);

	int i;
	pid_t pid;
	for (i = 0; i < 2; i++)
	{
		if ((pid = fork()) < 0)
		{
			fprintf(stderr, "Unable to create child process %d\n", i+1);
			exit(-1);
		}
		else if (pid == 0)
		{
			fprintf(stdout, "Created child process %d, PID: %d\n", i+1, getpid());
			switch (i)
			{
				case 0:
				{
					//Executing the first child process, the incrementer
					puts("\tIn incrementer");
					
					//Allocate buffer space for the incoming bitstring plus one for newline character
					char incoming[line_length + 1];

					while(1)
					{
						int received = read(fd_one[0], incoming, sizeof(incoming)); //Might need to add one here

						//While there are still bytes being read
						if (received != 0)
						{
							fprintf(stdout, "Incrementer received: %s,\t", incoming);
							//Create array to contain incremented value to send
							char outgoing[line_length+1];
							increment_number(incoming, outgoing);
							fprintf(stdout, "Incrementer sent: %s\n", outgoing);
							write(fd_two[1], outgoing, sizeof(outgoing)); //Might need to add 1 here
						}
						//Leave the process when there are no more bytes to read from the buffer
						else
						{
							break;
						}
					}
					break;
				}
				case 1:
				{
					//Executing the second child process, the adder
					puts("\tIn adder");


					break;
				}
			}
			exit(0);			
		}
	}

	//The parent process acts as the first stage of the processor
	char in_buffer[line_length + 1];
	char to_pipe[line_length + 1];

	//close(fd_one[0]);
	//close(fd_two[0]);
	//close(fd_two[1]);

	file_in_2 = fopen(input_b, "r");
	fprintf(stdout, "Waiting to process... press Ctrl^C to begin\n");

	while (delayed)
	{
		;
	}

	while (fgets(in_buffer, line_length + 2, file_in_2) != NULL)
	{
		fprintf(stdout, "Complementer received: %s,\t", in_buffer);
		complement_number(in_buffer, to_pipe, line_length);
		fprintf(stdout, "Complementer sent: %s,\n", to_pipe);
		write(fd_one[1], to_pipe, sizeof(to_pipe));
	}
	
	//close(fd_one[1]);


}

/******************************************
* 
******************************************/
void parse_command_line(int argc, char *argv[])
{
	//Command line argument checking and error reporting
	if (argc != 5)
	{
		fprintf(stderr, "Error initializing program\n");
		fprintf(stderr, "Usage: ./svp InputA, InputB, BitStringLength, NumberOfLines\n");
		exit(-1);
	}

	//Create file pointers for input files from argument names
	input_a = argv[1];
	input_b = argv[2];
	fprintf(stdout, "InputA is: %s\n", input_a);
	fprintf(stdout, "InputB is: %s\n", input_b);

	//Parse bitstring length and number of lines from arguments
	line_length = atoi(argv[3]);
	num_lines = atoi(argv[4]);
	fprintf(stdout, "BitStringLength is: %d\n", line_length);
	fprintf(stdout, "NumberOfLines is: %d\n", num_lines);
}

/******************************************
* 
******************************************/
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

/******************************************
* 
******************************************/
void handle_delay(int num)
{
	if (delayed == 1)
	{
		delayed = 0;
	}
}