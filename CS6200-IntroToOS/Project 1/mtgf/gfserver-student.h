/*
 *  This file is for use by students to define anything they wish.  It is used by the gf server implementation
 */
#ifndef __GF_SERVER_STUDENT_H__
#define __GF_SERVER_STUDENT_H__

#include <stdio.h>

#include "gf-student.h"
#include "gfserver.h"

typedef struct{
	gfcontext_t *ctx;
	const char *path;
}request_t;

typedef struct{
	const char *path;
	pthread_mutex_t lock;
}file_lock_t;

request_t *createRequest();

file_lock_t *createFileLock(const char * path);

void createWorkers(int nthreads);

void multithreadSetup();

void *workerSendFiles();

//void checkOpenFiles(const char* path);

//void unlockFile(const char *path);

#endif // __GF_SERVER_STUDENT_H__