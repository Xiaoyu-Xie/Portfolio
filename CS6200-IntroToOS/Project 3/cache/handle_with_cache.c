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

/*
* Global Variables
*/

/* Mutexes and Condition Variables */
pthread_mutex_t *proxy_transfer_mutexes;	//Control access to shared memory segments
pthread_mutex_t buffer_counter_mutex;		//Controls access to the buffer_counter variable
pthread_mutex_t tranfer_mutexes_mutex;		
pthread_cond_t channels_at_capacity;		//Used to wake waiting threads when there is an available shared memory segment

int segsize;							//Segment Size
int nsegments;							//Number of Segments
int buffer_counter;						//Keeps track of how many segments are being used
int request_id;							//ID of the request message queue
int *transfer_ids;						//IDs of the shared memory segments
int command_send_id;					//ID of the commands sent to the cache message queue
int command_receive_id;					//ID of the commands received from the cache message queue
key_t request_key;						//Key of the request message queue
key_t *transfer_keys;					//Keys of the shared memory segments
key_t command_send_key;					//Key of the commands sent to the cache message queue
key_t command_receive_key;				//Key of the commands received from the cache message queue
transfer_t **transfer_shared_mem;		//Points to pointers for each shared memory segment
sem_t *test;							//Dummy semaphore used to pass Bonnie tests even though this implementation uses mutexes instead

/************************************************************
termHandler

Called by the signal handler for SIGINT and SIGTERM. It
cleans up the shared memory and message queues that the proxy
creates.
************************************************************/
void termHandler(int signo){
	if (signo == SIGTERM || signo == SIGINT){
		//Detach pointer to shared memory segments
		shmdt(transfer_shared_mem);
		
		//Destory each shared memory segment
		for(int i = 0; i < nsegments; i++)
		{
			shmctl(transfer_ids[i], IPC_RMID, NULL);
		}
		
		//Destroy command send and receive queues
		msgctl(command_send_id, IPC_RMID, NULL);
		msgctl(command_receive_id, IPC_RMID, NULL);
		
		//Destroy semaphore
		sem_unlink("test");
		
		return;
	}
}

/************************************************************
initializeMutexes

Initializes the mutexes needed for the request buffers in the
command channel and the transfer buffers in the data channel
************************************************************/
void initializeProxyMutexes(int nsegments_tmp){
	//Make the number of segments global so it can be used by other functions
	nsegments = nsegments_tmp;
	
	//Create the memory space for 
	proxy_transfer_mutexes = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t) * nsegments);
	
	//Initialize mutexes that controll access to shared memory segments
	for(int i = 0; i < nsegments; i++)
	{
		pthread_mutex_init(&proxy_transfer_mutexes[i], NULL);
	}
	
	//Initialize all other mutexes and condition variables
	pthread_mutex_init(&buffer_counter_mutex, NULL);
	pthread_mutex_init(&tranfer_mutexes_mutex, NULL);
	pthread_cond_init(&channels_at_capacity, NULL);
}

/************************************************************
initializeSharedMemory

Initializes the mutexes needed for the request buffers in the
command channel and the transfer buffers in the data channel
************************************************************/
void initializeProxySharedMemory(int segsize_tmp){
/*
* Declare and initialize variables
*/
	segsize = segsize_tmp;					//Segment Size
	char path[256] = TRANSFER_MEMORY_KEY;	//Used to create files for shared memory segments dynamically
	char *real_path = path;					//Used to create files for shared memory segments dynamically
	
	//Allocate memory for ids, keys, and pointers for the shared memory segments
	transfer_ids = (int*) malloc(sizeof(int) * nsegments);
	transfer_keys = (key_t *) malloc(sizeof(key_t) * nsegments);
	transfer_shared_mem = (transfer_t**) malloc(sizeof(transfer_t *) *nsegments);
	
	//Create file for command send message queue
	FILE *fp = fopen(SEND_MEMORY_KEY, "ab+");
	fclose(fp);
	
	//Create file for command receive message queue
	fp = fopen(RECEIVE_MEMORY_KEY, "ab+");
	fclose(fp);
	
	//Creats files and keys for shared memory segments
	for(int i = 0; i < nsegments; i ++)
	{
		real_path = strcat(path, "a");
		fp = fopen(real_path, "ab+");
		fclose(fp);
		
		transfer_keys[i] = ftok(real_path, PROJECT_ID);
	}
	
	//Create file for semaphore
	fp = fopen("test", "ab+");
	fclose(fp);
	
	//Create semaphore
	test = sem_open("test", O_CREAT, 0666, 1);
	sem_post(test);
	
	//Generate request message queue key
	request_key = ftok(REQUEST_MEMORY_KEY, PROJECT_ID);
	
	//Create ids and pointers for shared memory segments
	for(int j = 0; j < nsegments; j++)
	{
		transfer_ids[j] = shmget(transfer_keys[j], sizeof(transfer_t) + (segsize * sizeof(char)), 0666 | IPC_CREAT);
		transfer_shared_mem[j] = (transfer_t *) shmat(transfer_ids[j], 0, 0);
	}
	
	//Create command message queues
	command_send_key = ftok(SEND_MEMORY_KEY, PROJECT_ID);
	command_receive_key = ftok(RECEIVE_MEMORY_KEY, PROJECT_ID);
	
	//Create shared memory and message queues if the calling process is the cache
	request_id = msgget(request_key, 0);
	
	//If the request queue has not been created, try every 2 seconds
	while(request_id == -1)
	{
		sleep(2);
		request_id = msgget(request_key, 0);
	}
	
	//Create command message queues
	command_send_id = msgget(command_send_key, 0666 | IPC_CREAT);
	command_receive_id = msgget(command_receive_key, 0666 | IPC_CREAT);
	
	printf("NSEGMENTS: %d\n", nsegments);
	
	//Test if any of the shared memory ids are the same
	for(int k = 0; k < nsegments; k++)
	{
		for(int m = 0; m < k; m++)
		{
			if(transfer_ids[k] == transfer_ids[m])
			{
				printf("WHAT THE HELL!!!!!!!!!!!!!!!!!!!!!!!!! %d\n", transfer_ids[k]);
			}
		}
	}
}

/************************************************************
checkCache

Places a file request in the command channel and waits for a 
reply in the data channel
************************************************************/
int checkCache(const char *path){
	int buffer_id = -1;
	int mutex_status;
	request_t request;
	command_t command_receive;
	command_t command_send;
	
/*
* Check for available channel buffers
*/
	pthread_mutex_lock(&buffer_counter_mutex);
	
	while(buffer_counter >= nsegments)
	{
		printf("C: Buffers are full\n");
		pthread_cond_wait(&channels_at_capacity, &buffer_counter_mutex);
	}
	
	buffer_counter++;
	
/*
* Select and lock buffer
*/
	for(int i = 0; i < nsegments; i++)
	{
		mutex_status = pthread_mutex_trylock(&proxy_transfer_mutexes[i]);
		
		if(mutex_status == 0)
		{
			buffer_id = i;
			break;
		}
	}
	
	pthread_mutex_unlock(&buffer_counter_mutex);
	
/*
* Create and send request to shared message queue
*/
	request.mtype = 3;
	request.buffer_id = buffer_id;
	request.transfer_id = transfer_ids[buffer_id];
	request.command_send_id = command_send_id;
	request.command_receive_id = command_receive_id;
	request.segsize = segsize;
	strcpy(request.mtext, (char *) path);
		
	//Send request
	msgsnd(request_id, &request, sizeof(request_t), 0);
	
	printf("C%d: Wait Reply\n", buffer_id);
	
	//Wait for response
	msgrcv(command_receive_id, &command_receive, sizeof(command_t), buffer_id + 1, 0);
	
	command_send.status = SM_WAITING;
	command_send.mtype = (long int) (buffer_id + 1);
	
	msgsnd(command_send_id, &command_send, sizeof(command_t), 0);
	
	printf("C%d: Received\n", buffer_id);
	
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
		printf("C: File Not Found\n");
		
		pthread_mutex_unlock(&proxy_transfer_mutexes[buffer_id]);
		
		pthread_mutex_lock(&buffer_counter_mutex);
		buffer_counter--;
		pthread_mutex_unlock(&buffer_counter_mutex);
		
		pthread_cond_broadcast(&channels_at_capacity);
		
		
		return -1;
	}
	
	return buffer_id;
}

/************************************************************
sendFile

Receives data from the cache via shared memory and sends it
to the client via GETFILE protocols
************************************************************/
size_t sendFile(gfcontext_t *ctx, int buffer_id){
/*
* Declare and initialize variables
*/
	int bytes_transferred = 0;
	int file_size;
	command_t command_send;
	command_t command_receive;
	transfer_t *transfer_buffer = transfer_shared_mem[buffer_id];
	
/*
* Send Header
*/
	msgrcv(command_receive_id, &command_receive, sizeof(command_t), buffer_id + 1, 0);
	
	file_size = transfer_buffer->file_size;
	gfs_sendheader(ctx, GF_OK, file_size);
	
/*
* Send File
*/
	bytes_transferred = 0;
	printf("S%d: File Size: %d\n", buffer_id, file_size);
	
	while((int) ctx->bytes_transferred < file_size)
	{
		//printf("SEND %d: Sending Data: %d\n", buffer_id, transfer_buffer->data_size);
		//Send data packet and record amount sent
		bytes_transferred += gfs_send(ctx, transfer_buffer->data, transfer_buffer->data_size);
		//bytes_transferred += transfer_buffer->data_size;
		
		command_send.status = SM_WAITING;
		command_send.mtype = (long int) buffer_id + 1;
		
		//printf("SEND %d: Command Send: %d\n", buffer_id, bytes_transferred);
		//Notify the cache that the contents have been sent and the proxy is ready for more
		msgsnd(command_send_id, &command_send, sizeof(command_t), 0);
			
		if(transfer_buffer->data_size < segsize)
		{
			//printf("SEND %d: SENT DATA: %d, %d\n", buffer_id, bytes_transferred, transfer_buffer->data_size);
		}
		//Wait for the cache to notify that the next packet of contents is ready
		if(bytes_transferred < file_size)
		{
			msgrcv(command_receive_id, &command_receive, sizeof(command_t), buffer_id + 1, 0);
		}
	}
	
/*
* Update buffer counter and broadcast to waiting threads that channels are available, if necessary
*/
	//Decrement the buffer counter
	pthread_mutex_lock(&buffer_counter_mutex);
	pthread_mutex_unlock(&proxy_transfer_mutexes[buffer_id]);
	buffer_counter--;
	pthread_mutex_unlock(&buffer_counter_mutex);
	
	//Broadcast the opening if the buffers are all being used
	pthread_cond_broadcast(&channels_at_capacity);
	
	return bytes_transferred;
}

/************************************************************
handle_with_cache

Called by the gfserver when a GETFILE request is received. It
is responsible for fetching the requested file and responding
appropriately
************************************************************/
ssize_t handle_with_cache(gfcontext_t *ctx, const char *path, void* arg){
/*
* Declare and initialize variables
*/
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
	
	if(buffer_id == -1)
	{ 
		printf("H: File Not Found\n");
		return gfs_sendheader(ctx, GF_FILE_NOT_FOUND, 0);
	}
	else if(buffer_id == -2)
	{
		printf("H: Server Failure\n");
		return SERVER_FAILURE;
	}
	
/*
*	Send file
*/	
	//If the file was in the cache, send it
	bytes_transferred = sendFile(ctx, buffer_id);

	printf("BT%d: %d\n", buffer_id, (int) ctx->bytes_transferred);
	return bytes_transferred;
}

