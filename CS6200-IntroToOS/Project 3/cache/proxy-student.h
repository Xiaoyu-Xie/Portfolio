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
#include <pthread.h>

#include "steque.h"
 
#define	REQUEST_BUFSIZE 300
#define TRANS_BUFSIZE 1024
#define MAXBUFFERS 16

#define REQUEST_MEMORY_KEY "requests"
#define TRANSFER_MEMORY_KEY "b"
#define SEND_MEMORY_KEY "send"
#define RECEIVE_MEMORY_KEY "receive"
#define PROJECT_ID 'D'

/* Used by the proxy and cache to indicate the state of the shared memory */
typedef enum _status_t{
	SM_WAITING,
	SM_FILE_NOT_FOUND,
	SM_ERROR,
	SM_READY,
	SM_DONE
}status_t;

/* Used to hold transfer data and transfer information in shared memory */
typedef struct _transfer_t {
	int file_size;
	int data_size;
	char data[1];
}transfer_t;

/* Used to request data from the cache */
typedef struct _request_t {
	long int mtype;
	int transfer_id;
	int command_send_id;
	int command_receive_id;
	int segsize;
	int buffer_id;
	char mtext[REQUEST_BUFSIZE];
}request_t;

/* Used by the proxy and cache to communicate during the transfer */
typedef struct _command_t {
	long int mtype;
	status_t status;
}command_t;

/* Structure for holding various data transfer channels */
typedef struct _transfer_buffers_t {
	transfer_t buffers[MAXBUFFERS];
}transfer_buffers_t;

#endif // __CACHE_STUDENT_H__