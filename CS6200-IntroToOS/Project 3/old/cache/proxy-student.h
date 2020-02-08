/*
 *  This file is for use by students to define anything they wish.  It is used by the proxy cache implementation
 */
#ifndef __CACHE_STUDENT_H__
#define __CACHE_STUDENT_H__
 
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h> 
#include <sys/sem.h>
#include <sys/ipc.h> 
#include <sys/msg.h>
 
#define	REQUEST_BUFSIZE 256
#define TRANS_BUFSIZE 1024
#define MAXBUFFERS 16

#define REQUEST_MEMORY_KEY "requests"
#define TRANSFER_MEMORY_KEY "transfers"
#define BUFFER_MEMORY_KEY "buffer"
#define PROJECT_ID 'D'
 
pthread_mutex_t proxy_transfer_mutexes[MAXBUFFERS];
pthread_mutex_t cache_transfer_mutexes[MAXBUFFERS];
pthread_mutex_t buffer_counter_mutex;

pthread_cond_t channels_at_capacity;

int buffer_counter;
int request_id;
int transfer_id;
key_t request_key;
key_t transfer_key;
request_t *request_shared_mem;
transfer_buffers_t *transfer_shared_mem;


enum status_t{
	SM_WAITING,
	SM_FILE_NOT_FOUND,
	SM_ERROR,
	SM_READY
};

enum role_t{
	R_PROXY,
	R_CACHE
};

typedef struct _transfer_t {
	int filesize;
	status_t status;
	char buffer[TRANS_BUFSIZE];
}transfer_t;
 
typedef struct _request_t {
	char path [REQUEST_BUFSIZE];
	int buffer_id;
}request_t;

typedef struct _transfer_buffers_t {
	struct transfer_t buffers[MAXBUFFERS];
}transfer_buffers_t;

typedef struct _command_buffers_t {
	role_t buffers[TRANS_BUFSIZE];
}command_buffers_t;

/************************************************************
initializeMutexes (Proxy)

Initializes the mutexes needed for the request buffers in the
command channel and the transfer buffers in the data channel
************************************************************/
void initializeMutexes(){
	for(int i = 0; i < MAXBUFFERS; i++)
	{
		pthread_mutex_init(&proxy_transfer_mutexes[i], NULL);
		pthread_mutex_init(&cache_transfer_mutexes[i], NULL);
	}
	
	pthread_mutex_init(&buffer_counter_mutex, NULL);
	pthread_cond_init(&channels_at_capacity, NULL);
}

/************************************************************
initializeSharedMemory (Both)

Initializes the mutexes needed for the request buffers in the
command channel and the transfer buffers in the data channel
************************************************************/
void initializeSharedMemory(role_t role){
	//Generate keys
	request_key = ftok(REQUEST_MEMORY_KEY, PROJECT_ID);
	transfer_key = ftok(TRANSFER_MEMORY_KEY, PROJECT_ID);
	
	//Create shared memory IDs and message queue
	if(role == R_PROXY)
	{
		request_id = msgget(request_key, 0);
		transfer_id = shmget(transfer_key, sizeof(transfer_buffers_t), 0);
	}
	else
	{
		request_id = msgget(request_key, 0);
		transfer_id = shmget(transfer_key, sizeof(transfer_buffers_t), IPC_CREAT);
	}
	
	//Get pointers to shared memory spaces
	transfer_shared_mem = (transfer_t *) shmat(transfer_id, NULL, 0);
}
 
/************************************************************
checkCache (Proxy)

Places a file request in the command channel and waits for a 
reply in the data channel
************************************************************/
int checkCache(char *path){
	int buffer_id = -1;
	int mutex_status;
	request_t request;
	transfer_t *transfer_buffer;
	
/*
* Check for available channel buffers
*/
	pthread_mutex_lock(&buffer_counter_mutex);
	
	while(buffer_counter >= MAXBUFFERS)
	{
		pthread_cond_wait(&channels_at_capacity, &buffer_counter_mutex);
	}
	
	buffer_counter++;
	
	pthread_mutex_unlock(&buffer_counter_mutex);
	
/*
* Select and lock buffer
*/
	for(int i = 0; i < MAXBUFFERS; i++)
	{
		mutex_status = pthread_mutex_trylock(proxy_transfer_mutexes[i]);
		
		if(mutex_status == 0)
		{
			buffer_id = i;
			transfer_buffer = (transfer_t *) &(transfer_shared_mem->buffers[i]);
			transfer_buffer->status = SM_WAITING;
			break;
		}
	}
	
/*
* Create and send request to shared message queue
*/
	//Create message request
	request.path = path;
	request.buffer_id = buffer_id;
	
	//Send request
	msgsnd(request_id, &request, sizeof(request_t), 0);
	
	//Wait for response
	while(transfer_buffer->status == SM_WAITING)
	{
		wait(2);
	}
	
	//Send file
	sendFile(transfer_buffer);
}

/************************************************************
checkCache (Proxy)

Places a file request in the command channel and waits for a 
reply in the data channel
************************************************************/
void sendFile(transfer_t *transfer_buffer){
	
}

#endif // __CACHE_STUDENT_H__