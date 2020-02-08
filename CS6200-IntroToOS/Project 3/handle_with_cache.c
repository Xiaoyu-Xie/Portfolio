#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h> 
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <semaphore.h>

#include "gfserver.h"
#include "proxy-student.h"

#define BUFSIZE 12041
#define WATCH 0

pthread_mutex_t *proxy_transfer_mutexes;
pthread_mutex_t buffer_counter_mutex;
pthread_mutex_t tranfer_mutexes_mutex;

pthread_cond_t channels_at_capacity;

int segsize;
int nsegments;
int buffer_counter;
int request_id;
int transfer_id;
int *command_send_ids;
int *command_receive_ids;
key_t request_key;
key_t transfer_key;
key_t *command_send_keys;
key_t *command_receive_keys;
transfer_t *transfer_shared_mem;
sem_t *test;


void termHandler(int signo){
	if (signo == SIGTERM || signo == SIGINT){
		// you should do IPC cleanup here
		
		msgctl(request_id, IPC_RMID, NULL);
		
		shmdt(transfer_shared_mem);
		shmctl(transfer_id, IPC_RMID, NULL);
		
		for(int i = 0; i < nsegments; i++)
		{	
			msgctl(command_send_ids[i], IPC_RMID, NULL);
			msgctl(command_receive_ids[i], IPC_RMID, NULL);
		}
		
		sem_unlink("test");
		
		return;
	}
}

/************************************************************
initializeMutexes (Proxy)

Initializes the mutexes needed for the request buffers in the
command channel and the transfer buffers in the data channel
************************************************************/
void initializeProxyMutexes(int nsegments_tmp){
	nsegments = nsegments_tmp;
	
	proxy_transfer_mutexes = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t) * nsegments);
	
	for(int i = 0; i < nsegments; i++)
	{
		pthread_mutex_init(&proxy_transfer_mutexes[i], NULL);
	}
	
	pthread_mutex_init(&buffer_counter_mutex, NULL);
	pthread_mutex_init(&tranfer_mutexes_mutex, NULL);
	pthread_cond_init(&channels_at_capacity, NULL);
	
	
}

/************************************************************
initializeSharedMemory (Both)

Initializes the mutexes needed for the request buffers in the
command channel and the transfer buffers in the data channel
************************************************************/
void initializeProxySharedMemory(int segsize_tmp){
	char modifier = 'a';
	segsize = segsize_tmp;
	
	command_send_ids = (int *) malloc(sizeof(int) * nsegments);
	command_receive_ids = (int *) malloc(sizeof(int) * nsegments);
	command_send_keys = (key_t *) malloc(sizeof(key_t) * nsegments);
	command_receive_keys = (key_t *) malloc(sizeof(key_t) * nsegments);
	//transfer_shared_mem = (transfer_t*) malloc((sizeof(transfer_t) + segsize) *nsegments);
	
	FILE *fp = fopen(SEND_MEMORY_KEY, "ab+");
	fclose(fp);
	
	fp = fopen(RECEIVE_MEMORY_KEY, "ab+");
	fclose(fp);
	
	fp = fopen(REQUEST_MEMORY_KEY, "ab+");
	fclose(fp);
	
	fp = fopen(TRANSFER_MEMORY_KEY, "ab+");
	fclose(fp);
	
	fp = fopen("test", "ab+");
	fclose(fp);
	
	test = sem_open("test", O_CREAT, 0666, 1);
	sem_post(test);
	
	//Generate keys
	request_key = ftok(REQUEST_MEMORY_KEY, PROJECT_ID);
	transfer_key = ftok(TRANSFER_MEMORY_KEY, PROJECT_ID);
	
	for(int i = 0; i < nsegments; i++)
	{
		command_send_keys[i] = ftok((SEND_MEMORY_KEY + modifier), PROJECT_ID);
		command_receive_keys[i] = ftok((RECEIVE_MEMORY_KEY + modifier), PROJECT_ID);
		//transfer_keys[i] = ftok((TRANSFER_MEMORY_KEY + modifier), PROJECT_ID);
		modifier += 'a';
	}
	
	//Create shared memory and message queues if the calling process is the cache
	request_id = msgget(request_key, 0666 | IPC_CREAT);
	transfer_id = shmget(transfer_key, nsegments * (sizeof(transfer_t) + segsize), 0666 | IPC_CREAT);
		
	for(int j = 0; j< nsegments; j++)
	{
		command_send_ids[j] = msgget(command_send_keys[j], 0666 | IPC_CREAT);
		command_receive_ids[j] = msgget(command_receive_keys[j], 0666 | IPC_CREAT);
	}
	
	transfer_shared_mem = (transfer_t *) shmat(transfer_id, 0, 0);
}

/************************************************************
checkCache (Proxy)

Places a file request in the command channel and waits for a 
reply in the data channel
************************************************************/
int checkCache(const char *path){
	//printf("CHECKCACHE: Checking Cache\n");
	int buffer_id = -1;
	int mutex_status;
	int local_buffer_count;
	request_t request;
	command_t command_receive;
	command_t command_send;
	
/*
* Check for available channel buffers
*/
	//printf("CHECKCACHE: Looking for Available Buffers\n");
	pthread_mutex_lock(&buffer_counter_mutex);
	
	while(buffer_counter >= nsegments)
	{
		printf("CHECKCACHE: Buffers are full\n");
		pthread_cond_wait(&channels_at_capacity, &buffer_counter_mutex);
	}
	
	buffer_counter++;
	
/*
* Select and lock buffer
*/
	//printf("CHECKCACHE: Locking Buffer\n");
	for(int i = 0; i < nsegments; i++)
	{
		mutex_status = pthread_mutex_trylock(&(proxy_transfer_mutexes[i]));
		
		if(mutex_status == 0)
		{
			buffer_id = i;
			break;
		}
	}
	
	pthread_mutex_unlock(&buffer_counter_mutex);
	
	printf("CHECKCACHE: Locked Buffer: %d\n", buffer_id);
	
/*
* Create and send request to shared message queue
*/
	command_send_ids[buffer_id] = msgget(command_send_keys[buffer_id], 0666 | IPC_CREAT);
	command_receive_ids[buffer_id] = msgget(command_receive_keys[buffer_id], 0666 | IPC_CREAT);
	
	request.mtype = 3;
	request.buffer_id = buffer_id;
	request.transfer_id = transfer_id;
	request.command_send_id = command_send_ids[buffer_id];
	request.command_receive_id = command_receive_ids[buffer_id];
	request.segsize = segsize;
	strcpy(request.mtext, (char *) path);
		
	if(buffer_id == WATCH)
	{
		printf("CHECKCACHE: Sending Request\n");
	}
	//Send request
	msgsnd(request_id, &request, sizeof(request_t), 0);
	
	if(buffer_id == WATCH)
	{
		printf("CHECKCACHE: Waiting on Reply\n");
	}
	//Wait for response
	msgrcv(command_receive_ids[buffer_id], &command_receive, sizeof(command_t), 2, 0);
	
	command_send.status = SM_WAITING;
	command_send.mtype = (long int) 1;
	
	msgsnd(command_send_ids[buffer_id], &command_send, sizeof(command_t), 0);
	
	if(buffer_id == WATCH)
	{
		printf("CHECKCACHE: Reply Received\n");
	}
	
	//If the cache indicates an error, return -2
	//If the cache indicates the file was not found, return -1
	if(command_receive.status == SM_ERROR)
	{
		pthread_mutex_unlock(&proxy_transfer_mutexes[buffer_id]);
		pthread_mutex_lock(&buffer_counter_mutex);
		buffer_counter--;
		
		pthread_mutex_unlock(&buffer_counter_mutex);
		pthread_cond_broadcast(&channels_at_capacity);
		
		return -2;
	}
	else if(command_receive.status == SM_FILE_NOT_FOUND)
	{
		printf("CHECKCACHE: File Not Found\n");
		
		pthread_mutex_unlock(&proxy_transfer_mutexes[buffer_id]);
		
		pthread_mutex_lock(&buffer_counter_mutex);
		local_buffer_count = buffer_counter;
		buffer_counter--;
		pthread_mutex_unlock(&buffer_counter_mutex);
		
		if(local_buffer_count == nsegments)
		{
			pthread_cond_broadcast(&channels_at_capacity);
		}
		
		
		return -1;
	}
	
	if(buffer_id == WATCH)
	{
		printf("CHECKCACHE: Exiting\n");
	}
	return buffer_id;
}

/************************************************************
sendFile (Proxy)

Receives data from the cache via shared memory and sends it
to the client via GETFILE protocols
************************************************************/
size_t sendFile(gfcontext_t *ctx, int buffer_id){
/*
* Declare and initialize variables
*/
	if(buffer_id == WATCH)
	{
		printf("SEND: Sending\n");
	}
	int bytes_transferred = 0;
	int file_size;
	int local_buffer_count;
	command_t command_send;
	command_t command_receive;
	transfer_t *transfer_buffer = &(transfer_shared_mem[buffer_id]);
	
/*
* Send Header
*/
	msgrcv(command_receive_ids[buffer_id], &command_receive, sizeof(command_t), 2, 0);
	
	if(buffer_id == WATCH)
	{
		printf("SEND: Sending Header\n");
	}
	file_size = transfer_buffer->file_size;
	gfs_sendheader(ctx, GF_OK, file_size);
	
/*
* Send File
*/
	bytes_transferred = 0;
	printf("SEND %d: Sending File Size: %d\n", buffer_id, file_size);
	
	while(bytes_transferred < file_size)
	{
		//printf("SEND %d: Sending Data: %d\n", buffer_id, transfer_buffer->data_size);
		//Send data packet and record amount sent
		gfs_send(ctx, transfer_buffer->data, transfer_buffer->data_size);
		bytes_transferred += transfer_buffer->data_size;
		
		command_send.status = SM_WAITING;
		command_send.mtype = (long int) 1;
		
		//printf("SEND %d: Command Send: %d\n", buffer_id, bytes_transferred);
		//Notify the cache that the contents have been sent and the proxy is ready for more
		msgsnd(command_send_ids[buffer_id], &command_send, sizeof(command_t), 0);
			
		if(transfer_buffer->data_size < segsize)
		{
			printf("SEND %d: SENT DATA: %d, %d\n", buffer_id, bytes_transferred, transfer_buffer->data_size);
		}
		//Wait for the cache to notify that the next packet of contents is ready	
		msgrcv(command_receive_ids[buffer_id], &command_receive, sizeof(command_t), 2, 0);
		
		//printf("SEND: Bytes Sent: %d\n", bytes_transferred);
	}
	
	//Done with the transfer buffer so release the mutex
	pthread_mutex_unlock(&(proxy_transfer_mutexes[buffer_id]));
	
/*
* Update buffer counter and broadcast to waiting threads that channels are available, if necessary
*/
	//Decrement the buffer counter
	pthread_mutex_lock(&buffer_counter_mutex);
	local_buffer_count = buffer_counter;
	buffer_counter--;
	pthread_mutex_unlock(&buffer_counter_mutex);
	
	//Broadcast the opening if the buffers are all being used
	if(local_buffer_count == nsegments)
	{
		pthread_cond_broadcast(&channels_at_capacity);
	}
	
	if(request_key == command_send_keys[buffer_id])
	{
		printf("HOLY SHIT!!!!!!!!\n");
	}
	
	msgctl(command_send_ids[buffer_id], IPC_RMID, NULL);
	msgctl(command_receive_ids[buffer_id], IPC_RMID, NULL);
	
	return bytes_transferred;
}

ssize_t handle_with_cache(gfcontext_t *ctx, const char *path, void* arg){
/*
* Declare and initialize variables
*/
	
	//printf("\nHANDLE: Entering Handle\n");
	size_t bytes_transferred;
	char buffer[TRANS_BUFSIZE];
	char *data_dir = arg;

	int buffer_id = -1;
	
	strncpy(buffer,data_dir, TRANS_BUFSIZE);
	strncat(buffer,path, TRANS_BUFSIZE);
	
/*
* Check with Cache
*/
	buffer_id = checkCache(path);
	
	if(buffer_id == WATCH)
	{
		printf("HANDLE: Checking with Cache\n");
	}
	
	if(buffer_id == -1)
	{ 
		printf("HANDLE: File Not Found\n");
		return gfs_sendheader(ctx, GF_FILE_NOT_FOUND, 0);
	}
	else if(buffer_id == -2)
	{
		printf("HANDLE: Server Failure\n");
		return SERVER_FAILURE;
	}
	
/*
*	Send file
*/	
	if(buffer_id == WATCH)
	{
		printf("HANDLE: Sending File\n");
	}
	//If the file was in the cache, send it
	bytes_transferred = sendFile(ctx, buffer_id);

	printf("HANDLE: Leaving Handle\nBytes Transferred %d: %d\n", buffer_id, (int) bytes_transferred);
	return bytes_transferred;
}

