/*
 *  This file is for use by students to define anything they wish.  It is used by the gf client implementation
 */
 #ifndef __GF_CLIENT_STUDENT_H__
 #define __GF_CLIENT_STUDENT_H__
 
 #include "gf-student.h"
 #include "gfclient.h"
 
 typedef struct{
	gfcrequest_t *gfr;
	char *path;
	FILE *file;
}request_t; 
 
 void multithreadSetup(int nthreads);
 
 void *workerReceiveFiles();
 
 /***************************************************************************************************
* Creates, initializes, and returns a request structure which holds the details of a client request
***************************************************************************************************/
request_t *createRequest();
 
 #endif // __GF_CLIENT_STUDENT_H__