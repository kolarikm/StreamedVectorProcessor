/*********************************************************
* Streamed Vector Processor
* CIS 452 Winter 2017
* Author: Michael Kolarik
*
* The program performs the subtraction of 2 binary numbers
* which are read in from external files. Three separate
* processes communicate using pipes - each performing a
* small piece of the overall operation.
*********************************************************/

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

//Store the name of the desired output file
char *output = "output.dat";

//Function declarations
void parse_command_line(int argc, char *argv[]);
void handle_delay();


/*********************************************************
* The main function acts as the parent process, as well as
* fulfilling the requirement of reading in one of the
* input files and performing a binary complement to the
* number before passing it through the first pipe to the
* child process responsible for incrementing the number.
*********************************************************/
int main(int argc, char *argv[])
{
	//Assign function to execute for interrupt signal
	signal(SIGINT, handle_delay);

	//Handle command line arguments
	parse_command_line(argc, argv);

	//Set up the pipes
	setup_pipes(fd_one, fd_two);

	//Fork logic used to spawn only 2 child processes
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
				//Executing the first child process, the incrementer
				case 0:
				{
					//Create variable space for the incoming bitstring plus one for newline character
					char incoming[line_length + 1];

					while(1)
					{
						//Read from the first pipe, and place it in the incoming array space
						int received = read(fd_one[0], incoming, sizeof(incoming)); 

						//While there are still bytes being read
						if (received != 0)
						{
							fprintf(stdout, "Incrementer received: %s\t", incoming);
							//Create array to contain incremented value to send
							char outgoing[line_length+1];
							increment_number(incoming, outgoing, line_length);
							fprintf(stdout, "Incrementer sent: %s\n", outgoing);
							write(fd_two[1], outgoing, sizeof(outgoing));
						}
						//Leave the process when there are no more bytes to read from the buffer
						else
						{
							break;
						}
					}
					break;
				}
				//Executing the second child process, the adder
				case 1:
				{
					//Create variables for both incoming from the pipe and reading from external input
					char incoming[line_length + 1];
					char in_buffer[line_length + 1];

					//Assign input and output files
					file_in_1 = fopen(input_a, "r");
					file_out = fopen(output, "w");

					while(1)
					{
						//Read from the second pipe, and place it in the incoming array space
						int received = read(fd_two[0], incoming, sizeof(incoming));

						//While there are lines to read, place the line into input buffer
						if (fgets(in_buffer, line_length + 3, file_in_1) == NULL)
						{
							break;
						}

						//While there are still bytes being read from pipe
						if (received != 0)
						{
							//Strip buffers of problematic newlines
							in_buffer[strcspn(incoming, "\r\n")] = 0;
							in_buffer[strcspn(in_buffer, "\r\n")] = 0;
							fprintf(stdout, "Adder received: %s\t", incoming);
							fprintf(stdout, "Adder read: %s\t", in_buffer);
							//Create array to contain added value to output
							char outgoing[line_length+1];
							add_numbers(incoming, in_buffer, outgoing, line_length);
							//Output added value to external file
							fprintf(file_out, "%s\n", outgoing);
							fprintf(stdout, "Adder writing: %s\n", outgoing);
							fflush(file_out);								
						}
						//Leave the process when there are no more bytes to read from the buffer
						else
						{
							break;
						}
					}
					fclose(file_out);
					fclose(file_in_1);	
					break;
				}
			}
			exit(0);			
		}
	}

	//The parent process acts as the first stage of the processor
	char in_buffer[line_length + 1];
	char to_pipe[line_length + 1];

	//Assigning input file for complement process
	file_in_2 = fopen(input_b, "r");
	fprintf(stdout, "Waiting to process... press Ctrl^C to begin\n");

	//Busy-wait until user signals processing to begin
	while (delayed)
	{
		;
	}

	fprintf(stdout, "\nStarting processing...\n");

	//Read data from input file until all lines are complete
	while (fgets(in_buffer, line_length + 3, file_in_2) != NULL)
	{
		in_buffer[strcspn(in_buffer, "\r\n")] = 0;
		fprintf(stdout, "Complementer received: %s\t", in_buffer);
		complement_number(in_buffer, to_pipe, line_length);
		fprintf(stdout, "Complementer sent: %s\n", to_pipe);
		write(fd_one[1], to_pipe, sizeof(to_pipe));
	}

	//Close all file descriptors and open files
	close(fd_one[0]);
	close(fd_one[1]);
	close(fd_two[0]);
	close(fd_two[1]);
	fclose(file_in_2);
}


/*********************************************************
* 
*********************************************************/
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

/*********************************************************
* 
*********************************************************/
void handle_delay()
{
	if (delayed == 1)
	{
		delayed = 0;
	}
}