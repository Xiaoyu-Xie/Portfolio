#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <fcntl.h>

#include "gfclient.h"
#include "gfclient-student.h"

#define BUFSIZE 103

struct gfcrequest_t{
	//Receive Statistics
	size_t bytes_received;
	size_t filelen;
	
	//Connection configuration
	char *server;
	unsigned short port;
	
	//Function properties
	char *path;
	gfstatus_t status;
	void (*writefunction)(void*,size_t,void*);
	void *writearguments;
	void (*headerfunction)(void*,size_t,void*);
	void *headerarguments;
};

// optional function for cleaup processing.
void gfc_cleanup(gfcrequest_t *gfr){
	free(gfr);
}

gfcrequest_t *gfc_create(){
    //Create, initialize, and return a gfcrequest_t pointer
	gfcrequest_t *gfcrequest;
	
	if((gfcrequest = malloc(sizeof(gfcrequest_t))) != NULL)
	{
		//Initialize receive statistics
		gfcrequest->bytes_received = 0;
		gfcrequest->filelen = 0;
		
		//Initialize connection configuration
		gfcrequest->server = "localhost";
		gfcrequest->port = 12041;
		
		//Initialize function properties
		gfcrequest->path = "/";
		gfcrequest->status = GF_OK;
		gfcrequest->writefunction = NULL;
		gfcrequest->writearguments = NULL;
		gfcrequest->headerfunction = NULL;
		gfcrequest->headerargurments = NULL;
	}

    return gfcrequest;
}

size_t gfc_get_bytesreceived(gfcrequest_t *gfr){
    return gfr->bytes_received;
}

size_t gfc_get_filelen(gfcrequest_t *gfr){
    return gfr->filelen;
}

gfstatus_t gfc_get_status(gfcrequest_t *gfr){
    return gfr->status;
}

void gfc_global_init(){
	return;
}

void gfc_global_cleanup(){
	return;
}

int gfc_perform(gfcrequest_t *gfr){
    /*
	* Intialize
	*/
	struct addrinfo hints;				//Server connection properties
	struct addrinfo *srvinfo;			//Server connection information: contains properties, ip, and port
	char portstr[10] = {0};				//Port number to connect to remote server on
	char message[BUFSIZE] = {0};		//GET request message sent to the server
	char message2[BUFSIZE] = {0};
	char content2[BUFSIZE] = {0};
	char *content;
	char buffer[BUFSIZE] = {0};			//Stores bytes received from recv commands
	char *scheme;
	char *response_status;
	char *filelength = NULL;
	char *header;
	int bytes_received = 0;				//Bytes received from each recv command
	int bytes_total = 0;				//Total bytes received from all recv commands
	int counter = 0;
	int header_length = 0;
	
	printf("\n\nInitialized Variables \n");
	
	//convert @portno into integer and store in @gfr->port
	sprintf(portstr, "%d", gfr->port);

	//Reserve memory for @hints
    memset(&hints, 0, sizeof(hints));
  
	//Initialize connection properties
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
  
	//Retrieve IP address for @hostname and store all information in @srvinfo
    getaddrinfo(gfr->server, portstr, &hints, &srvinfo);	//*EH	//Resolve hostname to IPv4

	/*
	* Connect to server
	*/
	int sock_id = socket(srvinfo->ai_family, srvinfo->ai_socktype, srvinfo->ai_protocol); //*EH	//Create socket
	//setsockopt(sock_id, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
	printf("Attempting Connection!\n");
	if(connect(sock_id, srvinfo->ai_addr, srvinfo->ai_addrlen) < 0) //*EH	//Connect to server
	{
		printf("Connection Could Not Be Made\n");
		gfr->status = GF_ERROR;
		freeaddrinfo(srvinfo);
		close(sock_id);
		return -1;
	}
	
	//Clean up
	freeaddrinfo(srvinfo);
	
	/*
	* Build and send request
	*/
	strcat(message, "GETFILE GET ");
	strcat(message, gfr->path);
	strcat(message, "\r\n\r\n");					//Build
	send(sock_id, message, strlen(message), 0);		//Send
	
	printf("\nMessage Sent: %s\n", message);
	
	//Initialize message holders
	message[0] = '\0';
	message2[0] = '\0';
	//message4[0] = '\0';
	buffer[0] = '\0';
	
	/*
	* Receive response message with content
	*/
	while (strstr(message, "\r\n\r\n") == NULL)
	{
		printf("TEST\n");
		
		if((int) strlen(message) > 100)
		{
			fprintf(stderr, "\nERROR: Did not receive termination string\n");
			gfr->status = GF_INVALID;
			close(sock_id);
			return -1;
		}
		
		bytes_received = recv(sock_id, buffer, BUFSIZE -1, 0);
		bytes_total = bytes_total + bytes_received;
		
		if(bytes_received <= 0)
		{
			fprintf(stderr, "\nERROR: Communication Timeout!\n");
			gfr->status = GF_INVALID;
			close(sock_id);
			return -1;
		}
		
		buffer[bytes_received] = '\0';
		strcat(message, buffer);
		
		buffer[0] = '\0';
		
	}
	
	//Receive statistics
	printf("Bytes Total: %d\n", bytes_total);
	message[bytes_total] = '\0';
	
	printf("Message Received: %s\n", message);
	
	//Separate header into variables
	strcpy(message2, message);
	//strcpy(message4, message);
	header = strtok(message, "\r");
	header_length = (int) strlen(header);
	
	if(bytes_total > header_length + 4)
	{
		content = header + header_length + 4;
	}
	else
	{
		content = NULL;
	}
	
	//strtok(NULL, "\n");
	//content = strtok(NULL, "");
	scheme = strtok(header, " ");
	response_status = strtok(NULL, " ");
	
	printf("HEADER: %s\n", header);
	printf("HEADER LENGTH: %d\n", header_length);
	printf("SCHEME: %s, STATUS: %s\n", scheme, response_status);
	
	//Calculate the number of bytes of content sent with the header
	if(content != NULL)
	{
		bytes_total = bytes_total - (header_length + 4);
		//memcpy(buffer, (message4 + (content - header)), bytes_total);
		printf("Content Calculation: %d\n", bytes_total);
	}
	else
	{
		bytes_total = 0;
	}
	
	/*if(response_status == NULL)
	{
		response_status = strtok(header, "");
	}*/
	
	/*
	* Header handling and validation
	*/
	//Invalid if no scheme or no response status were found
	if(scheme == NULL || response_status == NULL)
	{
		fprintf(stderr, "Terminating Connection: Header response invalid\n");
		gfr->status = GF_INVALID;
		close(sock_id);
		return -1;
	}
	
	if(!strcmp(response_status, "OK"))
	{
		
		strtok(message2, " ");
		strtok(NULL, " ");
		filelength = strtok(NULL, "\r\n\r\n");
		printf("File Length: %s\n", filelength);
		gfr->filelen = atoi(filelength);
	}
	
	//Invalid if the scheme isn't GETFILE or if the response status is not one of the approved statuses
	if(strcmp(scheme, "GETFILE") || (strcmp(response_status, "OK")&& strcmp(response_status, "ERROR")&&strcmp(response_status, "FILE_NOT_FOUND")&&strcmp(response_status, "INVALID")))
	{
		fprintf(stderr, "Terminating Connection: Header response invalid!\n");
		gfr->status = GF_INVALID;
		close(sock_id);
		return -1;
	}
	//Invalid if a file length is returned when the status is FILE_NOT_FOUND or ERROR
	else if((!strcmp(response_status, "FILE_NOT_FOUND") || !strcmp(response_status, "ERROR")) && filelength != NULL)
	{
		fprintf(stderr, "Terminating Connection: Header response invalid!!\n");
		gfr->status = GF_INVALID;
		close(sock_id);
		return -1;
	}
	//Status is FILE_NOT_FOUND if the header is valid and the response status is FILE_NOT_FOUND
	else if(!strcmp(response_status, "FILE_NOT_FOUND"))
	{
		fprintf(stderr, "Terminating Connection: File not found\n");
		gfr->status = GF_FILE_NOT_FOUND;
		close(sock_id);
		return 0;
	}
	//Status is ERROR if the header is valid and the response status is ERROR
	else if(!strcmp(response_status, "ERROR"))
	{
		fprintf(stderr, "Terminating Connection: Error returned by server\n");
		gfr->status = GF_ERROR;
		close(sock_id);
		return 0;
	}
	else if(!strcmp(response_status, "INVALID"))
	{
		fprintf(stderr, "Terminating Connection: Invalid header sent to server\n");
		gfr->status = GF_INVALID;
		close(sock_id);
		return -1;
	}
	
	//Write content to to file
	if(content != NULL)
	{	
		memcpy(content2, content, bytes_total);

		printf("Wrote content from header response: %d bytes\n", bytes_total);
		printf("Content: %s\n", (message + (content - header)));
		
		gfr->writefunction((void *) content2, bytes_total, gfr->writearguments);
		
		//If the content size is the same as the total file size, close the connection
		if(bytes_total >= gfr->filelen)
		{
			printf("ALL CONTENT IN HEADER\n");
			close(sock_id);
			gfr->bytes_received = bytes_total;
			gfr->status = GF_OK;
			return 0;
		}
	
	}
	
	buffer[0] = '\0';
	message[0] = '\0';
	
	/*
	* Receive results
	*/
	
	printf("Reading the body\n");
	
	for (;;) 
	{
		
		//Read message from server
		bytes_received = recv(sock_id, buffer, BUFSIZE - 1,0);
		if(bytes_received >= 0)
		{
			bytes_total += bytes_received;
		}
		//printf("Bytes Total: %d\n", bytes_total);
		counter++;
		buffer[bytes_received] = '\0';
		
		if(bytes_received <= 0 && bytes_total < gfr->filelen)
		{
			gfr->writefunction(buffer, bytes_received, gfr->writearguments);
			fprintf(stderr, "\nERROR: Communication Timeout\n");
			gfr->status = GF_OK;
			close(sock_id);
			gfr->bytes_received = bytes_total;
			return -1;
		}
		//printf("Bytes Received: %d\n", bytes_received);
		
		//Exit when the server is finished transferring data and close @output
		if(bytes_total + 1 >= gfr->filelen)
		{
			gfr->writefunction(buffer, bytes_received, gfr->writearguments);
			break;
		}
		
		//Write message to @output and prepare for next message
		gfr->writefunction(buffer, bytes_received, gfr->writearguments);
		
		//printf("Bytes Written: %d\n", (int) strlen(buffer));
		
		buffer[0] = '\0';
		bytes_received = 0;
	}
	
	printf("Packets received: %d\n", counter);
	printf("Final packet size: %d\n", bytes_received);	
	printf("Finished reading body!!!!\n");
	
	//Exit Program
	close(sock_id);
	gfr->bytes_received = bytes_total;
	gfr->status = GF_OK;
	return 0;
	
}

void gfc_set_headerarg(gfcrequest_t *gfr, void *headerarg){
	gfr->headerarguments = headerarg;
	return;
}

void gfc_set_headerfunc(gfcrequest_t *gfr, void (*headerfunc)(void*, size_t, void *)){
	gfr->headerfunction = headerfunc;
	return;
}

void gfc_set_path(gfcrequest_t *gfr, char* path){
	gfr->path = path;
	return;
}

void gfc_set_port(gfcrequest_t *gfr, unsigned short port){
	gfr->port = port;
	return;
}

void gfc_set_server(gfcrequest_t *gfr, char* server){
	gfr->server = server;
	return;
}

void gfc_set_writearg(gfcrequest_t *gfr, void *writearg){
	gfr->writearguments = writearg;
	return;
}

void gfc_set_writefunc(gfcrequest_t *gfr, void (*writefunc)(void*, size_t, void *)){
	gfr->writefunction = writefunc;
	return;
}

const char* gfc_strstatus(gfstatus_t status){
    const char *strstatus = "UNKNOWN";

    switch (status)
    {
        default:
        break;

        case GF_OK: {
            strstatus = "OK";
        }
        break;
        
        case GF_ERROR: {
            strstatus = "ERROR";
        }
        break;

        case GF_FILE_NOT_FOUND: {
            strstatus = "FILE_NOT_FOUND";
        }
        break;

        case GF_INVALID: {
            strstatus = "INVALID";
        }
        break;
    }

    return strstatus;
}

