#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h> 
#include <sys/sem.h>

#include "gfserver.h"
#include "proxy-student.h"

#define BUFSIZE (12041)
#define REQUEST_MEMORY_KEY "requests"
#define TRANSFER_MEMORY_KEY "transfers"
#define PROJECT_ID 'D'
#define MAXBUFFERS 16

int mutex_sem;

ssize_t handle_with_cache(gfcontext_t *ctx, const char *path, void* arg){
/*
* Declare and initialize variables
*/
	int fildes;
	size_t file_len, bytes_transferred;
	ssize_t read_len, write_len;
	char buffer[BUFSIZE];
	char *data_dir = arg;
	struct stat statbuf;

	
	strncpy(buffer,data_dir, BUFSIZE);
	strncat(buffer,path, BUFSIZE);
	

/*
* Check with Cache
*/
	fildes = checkCache(path);

/*
* Open file and send response header
*/	
	
	if( 0 > (fildes){
		if (errno == ENOENT)
			/* If the file just wasn't found, then send FILE_NOT_FOUND code*/ 
			return gfs_sendheader(ctx, GF_FILE_NOT_FOUND, 0);
		else
			/* Otherwise, it must have been a server error. gfserver library will handle*/ 
			return SERVER_FAILURE;
	}

	/* Calculating the file size */
	if (0 > fstat(fildes, &statbuf)) {
		return SERVER_FAILURE;
	}

	file_len = (size_t) statbuf.st_size;

	gfs_sendheader(ctx, GF_OK, file_len);

/*
* Send file
*/
	
	/* Sending the file contents chunk by chunk. */
	bytes_transferred = 0;
	while(bytes_transferred < file_len){
		read_len = read(fildes, buffer, BUFSIZE);
		if (read_len <= 0){
			fprintf(stderr, "handle_with_file read error, %zd, %zu, %zu", read_len, bytes_transferred, file_len );
			return SERVER_FAILURE;
		}
		write_len = gfs_send(ctx, buffer, read_len);
		if (write_len != read_len){
			fprintf(stderr, "handle_with_file write error");
			return SERVER_FAILURE;
		}
		bytes_transferred += write_len;
	}

	return bytes_transferred;
}

