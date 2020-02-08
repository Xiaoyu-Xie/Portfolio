#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <getopt.h>
#include <sys/stat.h>
#include <pthread.h>

#include "gfclient.h"
#include "gfclient-student.h"
#include "workload.h"
#include "steque.h"

//Multithreading variables
steque_t work_queue;						//Queue of available transfer requests that are not being actively handled
pthread_t *threads = NULL;					//Array of threads available to handle transfer requests
pthread_mutex_t work_queue_lock;			//Lock set when thread needs to read a request from the work queue
pthread_mutex_t finished_lock;
pthread_cond_t queue_not_empty;				//Condition variable for threads waiting on an empty work queue to have requests
int finished = 0;

#define USAGE                                                                 \
"usage:\n"                                                                    \
"  webclient [options]\n"                                                     \
"options:\n"                                                                  \
"  -h                  Show this help message\n"                              \
"  -n [num_requests]   Requests download per thread (Default: 4)\n"           \
"  -p [server_port]    Server port (Default: 12041)\n"                         \
"  -s [server_addr]    Server address (Default: 127.0.0.1)\n"                   \
"  -t [nthreads]       Number of threads (Default 32)\n"                       \
"  -w [workload_path]  Path to workload file (Default: workload.txt)\n"       \

/* OPTIONS DESCRIPTOR ====================================================== */
static struct option gLongOptions[] = {
  {"help",          no_argument,            NULL,           'h'},
  {"nthreads",      required_argument,      NULL,           't'},
  {"nrequests",     required_argument,      NULL,           'n'},
  {"server",        required_argument,      NULL,           's'},
  {"port",          required_argument,      NULL,           'p'},
  {"workload-path", required_argument,      NULL,           'w'},
  {NULL,            0,                      NULL,             0}
};

static void Usage() {
	fprintf(stderr, "%s", USAGE);
}

static void localPath(char *req_path, char *local_path){
  static int counter = 0;

  sprintf(local_path, "%s-%06d", &req_path[1], counter++);
}

static FILE* openFile(char *path){
  char *cur, *prev;
  FILE *ans;

  /* Make the directory if it isn't there */
  prev = path;
  while(NULL != (cur = strchr(prev+1, '/'))){
    *cur = '\0';

    if (0 > mkdir(&path[0], S_IRWXU)){
      if (errno != EEXIST){
        perror("Unable to create directory");
        exit(EXIT_FAILURE);
      }
    }

    *cur = '/';
    prev = cur;
  }

  if( NULL == (ans = fopen(&path[0], "w"))){
    perror("Unable to open file");
    exit(EXIT_FAILURE);
  }

  return ans;
}

/* Callbacks ========================================================= */
static void writecb(void* data, size_t data_len, void *arg){
  FILE *file = (FILE*) arg;

  fwrite(data, 1, data_len, file);
}

/* Main ========================================================= */
int main(int argc, char **argv) {
/* COMMAND LINE OPTIONS ============================================= */
/*
* 1. Declare and inititalize variables
*/
  int i = 0;
  int option_char = 0;
  int nrequests = 4;
  int nthreads = 32;
  //int returncode = 0;
  gfcrequest_t *gfr = NULL;
  FILE *file = NULL;
  char *req_path = NULL;
  char local_path[1033];

  //Connection Variables
  char *server = "localhost";
  unsigned short port = 12041;
  char *workload_path = "workload.txt";
  
  
  request_t *new_request;					//Holds the information needed for a worker thread to make a transfer request
  
  setbuf(stdout, NULL); // disable caching

/*
* 2. Assign command line arguments
*/
  // Parse and set command line arguments
  while ((option_char = getopt_long(argc, argv, "t:hn:xp:s:w:", gLongOptions, NULL)) != -1) {
    switch (option_char) {
      case 'h': // help
        Usage();
        exit(0);
        break;                      
      case 'n': // nrequests
        nrequests = atoi(optarg);
        break;
      case 'p': // port
        port = atoi(optarg);
        break;
      default:
        Usage();
        exit(1);
      case 's': // server
        server = optarg;
        break;
      case 't': // nthreads
        nthreads = atoi(optarg);
        break;
      case 'w': // workload-path
        workload_path = optarg;
        break;
    }
  }
 
/*
* 3. Inititialize workload
*/
  if( EXIT_SUCCESS != workload_init(workload_path)){
    fprintf(stderr, "Unable to load workload file %s.\n", workload_path);
    exit(EXIT_FAILURE);
  }

  gfc_global_init();
  
 /*
* 4. Create and start worker threads
*/
	multithreadSetup(nthreads);

/*
* 5. Generate requests and add to the work queue
*/
  /*Making the requests...*/
  for(i = 0; i < nrequests * nthreads; i++){
    req_path = workload_get_path();
	new_request = createRequest();

    if(strlen(req_path) > 500){
      fprintf(stderr, "Request path exceeded maximum of 500 characters\n.");
      exit(EXIT_FAILURE);
    }

    localPath(req_path, local_path);
	new_request->path = req_path;

    file = openFile(local_path);
	new_request->file = file;

    gfr = gfc_create();
    gfc_set_server(gfr, server);
    gfc_set_path(gfr, req_path);
    gfc_set_port(gfr, port);
    gfc_set_writefunc(gfr, writecb);
    gfc_set_writearg(gfr, file);
	
	new_request->gfr = gfr;

    fprintf(stdout, "Requesting %s%s\n", server, req_path);
	
	//Add request to work queue
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
	
	//printf("HANDLER: File Path: %s\n", req_path);

    /*if ( 0 > (returncode = gfc_perform(gfr))){
      fprintf(stdout, "gfc_perform returned an error %d\n", returncode);
      fclose(file);
      if ( 0 > unlink(local_path))
        fprintf(stderr, "warning: unlink failed on %s\n", local_path);
    }
    else {
        fclose(file);
    }

    if ( gfc_get_status(gfr) != GF_OK){
      if ( 0 > unlink(local_path))
        fprintf(stderr, "warning: unlink failed on %s\n", local_path);
    }

    fprintf(stdout, "Status: %s\n", gfc_strstatus(gfc_get_status(gfr)));
    fprintf(stdout, "Received %zu of %zu bytes\n", gfc_get_bytesreceived(gfr), gfc_get_filelen(gfr));

    gfc_cleanup(gfr);*/

  }
  
  pthread_mutex_lock(&finished_lock);
  finished = 1;
  pthread_mutex_unlock(&finished_lock);
  
  pthread_cond_broadcast(&queue_not_empty);

/*
* 6. Cleanup and exit
*/
  gfc_global_cleanup();
  pthread_exit(NULL);

  return 0;
}  

/**************************************************************************
* Function for worker threads to execute to send files to clients
**************************************************************************/
void *workerReceiveFiles()
{
/*
* 1. Declare and initialize variables
*/
	//int       file_length = 0;					//Length of the file being read
	//int       bytes_read = 0;					//Number of bytes from each read from the input file during the sending process
	//int       bytes_total = 0;					//Total number of bytes read from the input file during the sending process
	//int       input_file_descriptor;			//File descriptor for the file to read from
	//char      data_buffer[BUFSIZE] = {0};		//Holds constructed data for sending
	request_t *request;
	char local_path[1033];
	//struct    stat stat_buf;
	//char      local_path[1033];
	
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
			pthread_mutex_lock(&finished_lock);
			
			if(finished == 1)
			{
				pthread_mutex_unlock(&work_queue_lock);
				pthread_mutex_unlock(&finished_lock);
				pthread_exit(NULL);
			}
			
			pthread_mutex_unlock(&finished_lock);
			
			pthread_cond_wait(&queue_not_empty, &work_queue_lock);
		}
		
		//Once there are requests in the work queue, retrieve a request, unlock the work queue, then continue
		request = (request_t *) steque_pop(&work_queue);
		pthread_mutex_unlock(&work_queue_lock);
		
		localPath(request->path, local_path);

		if ( 0 > (gfc_perform(request->gfr))){
			//fprintf(stdout, "gfc_perform returned an error %d\n", returncode);
			fclose(request->file);
			unlink(local_path);
			//if ( 0 > unlink(local_path))
				//fprintf(stderr, "warning: unlink failed on %s\n", request->path);
		}
		else {
			fclose(request->file);
		}

		if ( gfc_get_status(request->gfr) != GF_OK){
			unlink(local_path);
			//if ( 0 > unlink(local_path))
				//fprintf(stderr, "warning: unlink failed on %s\n", request->path);
		}

    //fprintf(stdout, "Status: %s\n", gfc_strstatus(gfc_get_status(request->gfr)));
    //fprintf(stdout, "Received %zu of %zu bytes\n", gfc_get_bytesreceived(request->gfr), gfc_get_filelen(request->gfr));

		gfc_cleanup(request->gfr);
		
/*
* 3. Open file and check for errors
*/	
		/*//Get true file path from content map,
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
		file_length = stat_buf.st_size;*/
		
/*
* 4. Send header
*/
		/*gfs_sendheader(request->ctx, GF_OK, file_length);
		printf("HANDLER: Sent File Size: %d\n", file_length);*/

/*
* 5. Read data into buffer and send data
*/
		/*while(bytes_total != file_length)
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
		}*/

/*
* 6. Close
*/
		free(request);
	}
	
	pthread_exit(NULL);
}	

/*************************************************************************
* Initializes threads
*************************************************************************/
void multithreadSetup(int nthreads)
{
	//Declare and initialize variables
	pthread_mutex_init(&work_queue_lock, NULL);
	pthread_mutex_init(&finished_lock, NULL);
	pthread_cond_init(&queue_not_empty, NULL);
	
	//Allocate memory for threads 
	threads = malloc(sizeof(pthread_t) * nthreads);
	
	//Create threads
	for(int counter = 0; counter < nthreads; counter++)
	{
		pthread_create(&threads[counter], NULL, workerReceiveFiles, NULL);
	}
	
	return;
}

/***************************************************************************************************
* Creates, initializes, and returns a request structure which holds the details of a client request
***************************************************************************************************/
request_t *createRequest()
{
	request_t *new_request = malloc(sizeof(request_t));
	
	new_request->file = NULL;
	new_request->path = NULL;
	new_request->gfr = NULL;
	
	return new_request;
}