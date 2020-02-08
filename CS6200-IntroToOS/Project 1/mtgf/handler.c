#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

//Added by author
#include <pthread.h>

//Custom header files
#include "steque.h"
#include "gfserver.h"
#include "gfserver-student.h"
#include "content.h"

//Buffer size
#define BUFSIZE 12041

//GETFILE Statuses
#define  GF_OK 200
#define  GF_FILE_NOT_FOUND 400
#define  GF_ERROR 500

//Gobal Variables
int number_of_threads = 1;					//Number of threads available to send files
steque_t work_queue;						//Queue of available transfer requests that are not being actively handled
pthread_t *threads = NULL;					//Array of threads available to handle transfer requests
pthread_mutex_t work_queue_lock;			//Lock set when thread needs to read a request from the work queue
pthread_cond_t queue_not_empty;				//Condition variable for threads waiting on an empty work queue to have requests

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
	request_t *new_request;				//New request from client
	
	//Creates request and initializes it
	new_request = createRequest();
	new_request->ctx = ctx;
	new_request->path = path;
	
	//Sets lock on work queue, adds request to the queue, and unlocks the queue when done
	pthread_mutex_lock(&work_queue_lock);
	
	if(steque_isempty(&work_queue))
	{
		steque_push(&work_queue, (steque_item) new_request);
		pthread_cond_broadcast(&queue_not_empty);
	}
	else
	{
		steque_push(&work_queue, (steque_item) new_request);
	}
	
	pthread_mutex_unlock(&work_queue_lock);
	
	printf("HANDLER: File Path: %s\n", path);
	

	return 0;
}

/*****************************************************************************************
* Creates, inititalizes, and starts multithreaded components such as mutexes and threads
*****************************************************************************************/
void createWorkers(int nthreads)
{
	number_of_threads = nthreads;
	printf("HANDLER: Number of threads: %d\n", number_of_threads);
	multithreadSetup();
}

/*************************************************************************
* Initializes threads
*************************************************************************/
void multithreadSetup()
{
	//Declare and initialize variables
	pthread_mutex_init(&work_queue_lock, NULL);
	pthread_cond_init(&queue_not_empty, NULL);
	
	//Allocate memory for threads 
	threads = malloc(sizeof(pthread_t) * number_of_threads);
	
	//Create threads
	for(int counter = 0; counter < number_of_threads; counter++)
	{
		pthread_create(&threads[counter], NULL, workerSendFiles, NULL);
	}
	
	return;
}

/**************************************************************************
* Function for worker threads to execute to send files to clients
**************************************************************************/
void *workerSendFiles()
{
/*
* 1. Declare and initialize variables
*/
	int  file_length = 0;					//Length of the file being read
	int  bytes_read = 0;					//Number of bytes from each read from the input file during the sending process
	int  bytes_total = 0;					//Total number of bytes read from the input file during the sending process
	int  input_file_descriptor;				//File descriptor for the file to read from
	char data_buffer[BUFSIZE] = {0};		//Holds constructed data for sending
	request_t *request;						//Holds the request retrieved from the work queue
	struct stat stat_buf;					//Used to get the file length
	
/*
* 2. Choose Request
*/
	for(;;)
	{
		//If the work queue is empty go into a condition wait until there is work to be done
		pthread_mutex_lock(&work_queue_lock);
		
		//if(steque_isempty(&work_queue))
		while(steque_isempty(&work_queue))
		{
			pthread_cond_wait(&queue_not_empty, &work_queue_lock);
			
			if(steque_isempty(&work_queue))
			{
				//pthread_mutex_unlock(&work_queue_lock);
			}
		}
		
		//Once there are requests in the work queue, retrieve a request, unlock the work queue, then continue
		request = (request_t *) steque_pop(&work_queue);
		pthread_mutex_unlock(&work_queue_lock);

/*
* 3. Open file and check for errors
*/	
		//Get true file path from content map,
		input_file_descriptor = content_get(request->path);
		
		if(input_file_descriptor == -1)
		{
			printf("WORKER: File Not Found: %s\n", request->path);
			gfs_sendheader(request->ctx, GF_FILE_NOT_FOUND, file_length);
			free(request);
			bytes_total = 0;
			continue;
		}
		
		//Find length of the file and save it in @file_length
		fstat(input_file_descriptor, &stat_buf);
		file_length = stat_buf.st_size;
		
/*
* 4. Send header
*/
		gfs_sendheader(request->ctx, GF_OK, file_length);
		printf("HANDLER: Sent File Size: %d\n", file_length);

/*
* 5. Read data into buffer and send data
*/
		while(bytes_total != file_length)
		{
			bytes_read = read(input_file_descriptor, data_buffer, BUFSIZE);
			bytes_total += bytes_read;
			
			if(bytes_read <= 0)
			{
				bytes_total = file_length;
			}
			else
			{
				gfs_send(request->ctx, (void *) data_buffer, bytes_read);
				data_buffer[0] = '\0';
			}
		}

/*
* 6. Close
*/
		free(request);
		bytes_total = 0;
	}
	
	pthread_exit(NULL);
}	

/***************************************************************************************************
* Creates, initializes, and returns a request structure which holds the details of a client request
***************************************************************************************************/
request_t *createRequest()
{
	request_t *new_request = malloc(sizeof(request_t));
	new_request->path = NULL;
	new_request->ctx = NULL;
	
	return new_request;
}

/****************************************************************************************************
* Creates, initializes, and returns a mutex lock that prevents a file from being read simultaneously
****************************************************************************************************/
file_lock_t *createFileLock(const char * path)
{
	file_lock_t *lock = malloc(sizeof(file_lock_t));
	lock->path = path;
	
	return lock;
}

/************************************************************************************************************************
* Called by worker thread to acquire the lock for the file it wants to read. If there isn't one, it creates and locks it
************************************************************************************************************************/
/*void checkOpenFiles(const char* path)
{*/
/*
* 1. Lock the file lock array
*/
	//pthread_mutex_lock(&file_locks);

/*
* 2. Search for locks on the file. If found, lock the file
*/
	/*for(int counter = 0; counter < number_of_threads; counter++)
	{
		if(open_files[counter] != NULL)
		{
			if(open_files[counter]->path == path)
			{
				pthread_mutex_unlock(&file_locks);
				pthread_mutex_lock(&(open_files[counter]->lock));
				return;
			}
		}
	}*/
	
/*
* 3. If the file lock wasn't found, create a lock for the file
*/
	/*file_lock_t *file_lock = createFileLock(path);
	
	//Look for an empty pointer in the open files array and add the lock there
	for(int counter = 0; counter < number_of_threads; counter++)
	{
		if(open_files[counter] == NULL)
		{
			open_files[counter] = file_lock;
			pthread_mutex_lock(&file_lock->lock);
			pthread_mutex_unlock(&file_locks);
			return;
		}
	}
	
	//No empty pointers were found so use a slot occupied but not locked
	for(int counter = 0; counter < number_of_threads; counter++)
	{
		if(!pthread_mutex_trylock(&(open_files[counter]->lock)))
		{
			open_files[counter]->path = path;
			pthread_mutex_lock(&(open_files[counter]->lock));
			pthread_mutex_unlock(&file_locks);
			return;
		}
	}
}*/
	
/***********************************************************************
* Finds file lock and unlocks it
***********************************************************************/
/*void unlockFile(const char *path)
{
	//Find file lock and  unlock it
	for(int counter = 0; counter < number_of_threads; counter++)
	{
		if(open_files[counter]->path == path)
		{
			pthread_mutex_unlock(&file_locks);
			pthread_mutex_unlock(&(open_files[counter]->lock));
			return;
		}
	}
}*/
