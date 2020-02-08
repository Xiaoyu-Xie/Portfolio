#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

//Custom header files
#include "gfserver.h"
#include "gfserver-student.h"
#include "content.h"

//Buffer size
#define BUFSIZE 12041

//GETFILE Statuses
#define  GF_OK 200
#define  GF_FILE_NOT_FOUND 400
#define  GF_ERROR 500

//
//  The purpose of this function is to handle a get request
//
//  The ctx is the "context" operation and it contains connection state
//  The path is the path being retrieved
//  The arg allows the registration of context that is passed into this routine.
//  Note: you don't need to use arg. The test code uses it in some cases, but
//        not in others.
//
ssize_t gfs_handler(gfcontext_t *ctx, const char *path, void* arg){
/*
* 1. Declare and initialize variables
*/
	int  file_length = 0;					//Length of the file being read
	int  bytes_read = 0;					//Number of bytes from each read from the input file during the sending process
	int  bytes_total = 0;					//Total number of bytes read from the input file during the sending process
	int  input_file_descriptor;				//File descriptor for the file to read from
	FILE * input_file;						//File stream for calculating the file length
	char data_buffer[BUFSIZE] = {0};		//Holds constructed data for sending
	
	printf("HANDLER: File Path: %s\n", path);
	
/*
* 2. Open file and check for errors
*/
	//Get true file path from content map,
	input_file_descriptor = content_get(path);
	input_file = fdopen(input_file_descriptor, "rb");
	
	if(input_file == NULL)
	{
		printf("HANDLER: File Not Found: %s\n", path);
		gfs_sendheader(ctx, GF_FILE_NOT_FOUND, file_length);
	}
	
	//Find length of the file and save it in @file_length
	fseek(input_file, 0, SEEK_END);
	file_length = ftell(input_file);
	rewind(input_file);
	
/*
* 3. Send header
*/
	gfs_sendheader(ctx, GF_OK, file_length);

/*
* 4. Read data into buffer and send data
*/
	while(bytes_total != file_length)
	{
		bytes_read = read(input_file_descriptor, data_buffer, BUFSIZE);
		bytes_total += bytes_read;
		
		if(bytes_read <= 0)
		{
			fprintf(stderr, "ERROR: HANDLER: Could not continue reading from the file\n");
			gfs_abort(ctx);
			return bytes_total;
		}
		
		gfs_send(ctx, (void *) data_buffer, bytes_read);
		
		data_buffer[0] = '\0';
	}

/*
* 5. Close
*/
	return bytes_total;
}

