#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <fcntl.h>


#include "gfserver.h"
#include "gfserver-student.h"

#define BUFSIZE 1033

/* 
 * Modify this file to implement the interface specified in
 * gfserver.h.
 */
struct gfserver_t{
	//Connection configuration
	unsigned short port;
	int maxpending;
	
	//Function properties
	ssize_t (*handler)(gfcontext_t *, const char *, void*);
	void *handlerarguments;
};

struct gfcontext_t{
	char *path;
	int sock_id;
};
 
void gfs_abort(gfcontext_t *ctx){
	close(ctx->sock_id);
	return;
}

gfserver_t* gfserver_create(){
    //Create, initialize, and return a gfcrequest_t pointer
	gfserver_t *gfserver;
	
	if((gfserver = malloc(sizeof(gfserver_t))) != NULL)
	{
		//Initialize connection configuration
		gfserver->port = 12041;
		gfserver->maxpending = 5;
		
		//Initialize function properties
		gfserver->handler = NULL;
		gfserver->handlerarguments = NULL;
	}

    return gfserver;
}

ssize_t gfs_send(gfcontext_t *ctx, const void *data, size_t len){
	char *data_char = (char *) data;
	char message[BUFSIZE] = {0};
	int message_size = 0;
	int chunks = (len/BUFSIZE) + 1;

	printf("SEND: Data Length: %d   Chunks: %d\n", (int) len, chunks);
	
	for(int counter = 0; counter < chunks; counter++)
	{
		if(counter == (chunks - 1))
		{
			printf("SEND: Final Chunk\n");
			memcpy(message, &data_char[counter * BUFSIZE], (len - (counter * BUFSIZE)));
			message_size = (len - (counter * BUFSIZE));
		}
		else
		{
			memcpy(message, &data_char[counter * BUFSIZE], BUFSIZE);
			message_size = BUFSIZE;
		}
		
		if(send(ctx->sock_id, message, message_size, 0) < 0)
		{
			printf("ERROR: Data did not finish sending\n");
			close(ctx->sock_id);
			return -1;
		}
		
		message[0] = '\0';
	}
	
	printf("SEND: Finished!!\n");
	
    return len;
}

ssize_t gfs_sendheader(gfcontext_t *ctx, gfstatus_t status, size_t file_len){
    char strstatus[30] = {0};
	char str_file_len[30] = {0};
	char header_message[600] = {0};
	
	header_message[0] = '\0';
	
	strstatus[0] = '\0';
	snprintf(str_file_len, 10, "%d", ((int) file_len));
	
	//Send file not found response
	if(status == GF_FILE_NOT_FOUND)
	{
		printf("SENDHEADER: File Not Found\n");
		strcat(strstatus, "FILE_NOT_FOUND");
	}
	else if(status == GF_INVALID)
	{
		printf("SENDHEADER: Invalid\n");
		strcat(strstatus, "INVALID");
	}
	else if(status == GF_ERROR)
	{
		printf("SENDHEADER: Error\n");
		strcat(strstatus, "ERROR");
	}
	else if(status == GF_OK)
	{
		printf("SENDHEADER: OK\n");
		strcat(strstatus, "OK");
	}
	else
	{
		printf("\nERROR: Incorrect header status\n");
		return -1;
	}
	
	strcat(header_message, "GETFILE ");
	strcat(header_message, strstatus);
	
	if(!strcmp(strstatus, "OK"))
	{
		strcat(header_message, " ");
		strcat(header_message, str_file_len);
	}
	
	strcat(header_message, "\r\n\r\n");
	
	printf("SENDHEADER: Sending Header: %s\n", header_message);
	printf("SENDHEADER: Header Length: %d\n", (int) strlen(header_message));
	
	send(ctx->sock_id, header_message, strlen(header_message), 0);
	
	return 0;
}

void gfserver_serve(gfserver_t *gfs){
	/*
	* Declare and initialize variables
	*/
	int message_end = 0;						//Holds the amount of bytes read for each fread
    int new_socket;									//Socket descriptor for the socket used once the connection has been accepted
	int clientlen = sizeof(struct sockaddr_in);		//Size is needed to accept a connection
	int one = 1;									//The socket options require the address of an interger but for our purposes, it is not important
    int sock_id = socket(AF_INET, SOCK_STREAM, 6);	//Socket descriptor for the listening socket that is bound to the IP
	char message[300];					//Byte buffer for holding the message that is sent to the client
	char buffer[300];
	char *scheme;
	char *method;
	char *path;
	char serverpath[300] = "server_root";
	struct sockaddr_in listen_addr;					//Socket Address structure for holding the properties and settings of the listening socket
	gfcontext_t *ctx;
	
	/*
	* GFContext Creation
	*/
	ctx = malloc(sizeof(gfcontext_t));
	
	//Specifies that the listening port will use IPv4, and what port and IP it will use
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(gfs->port);
	listen_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	//Sets the options for the listening socket. Most importantly, that the ip address can be resused for simultaneous connections
	setsockopt(sock_id, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));
	
	//Binds a socket to the descriptor
    bind(sock_id, (struct sockaddr *) &listen_addr, sizeof(listen_addr));

	/*
	* Listen for connections then send the file in buffer size messages
	*/
    for(;;) {
		message[0] = '\0';
		buffer[0] = '\0';
		serverpath[0] = '\0';
		
		printf("\nListening....\n");
		
		//Begin listening
		listen(sock_id, gfs->maxpending);
		
		//Accept connection and assign it to a new socket descriptor
		new_socket = accept(sock_id, (struct sockaddr *) &listen_addr, (socklen_t*) &clientlen);
		ctx->sock_id = new_socket;
		
		printf("Connection Accepted\n");
		
		//Read request
		while(strstr(message, "\r\n\r\n") == NULL)
		{
			message_end = recv(new_socket, buffer, sizeof(buffer), 0);
			if(message_end == -1)
			{
				
				break;
			}
			buffer[message_end] = '\0';
			strcat(message, buffer);
			buffer[0] = '\0';
		}
		
		if(message_end == -1)
		{
			fprintf(stderr, "ERROR: Communication ended before request was received\n");
			close(sock_id);
			continue;
		}
	
		printf("Header Received: %s\n", message);
	
		//Save header components into variables
		scheme = strtok(message, " ");
		method = strtok(NULL, " ");
		path = strtok(NULL, "\r\n\r\n");
		
		printf("Scheme: %s,  Method: %s,  Path: %s\n", scheme, method, path);
		
		printf("Scheme: %s\n", scheme);
		if(scheme == NULL || method == NULL || path == NULL)
		{
			printf("Invalid Header\n");
			gfs_sendheader(ctx, GF_INVALID, 0);
			
			close(new_socket);
			continue;
		}
		
		//IF the header is incorrect, return the INVALID status and go back to listening
		if(strcmp(scheme, "GETFILE") || strcmp(method, "GET") || strncmp(path, "/", 1))
		{
			printf("Invalid Header!\n");
			
			gfs_sendheader(ctx, GF_INVALID, 0);
			
			
			//Close socket and start listening
			close(new_socket);
			continue;
		}
		
		if(path != NULL)
		{
			strcat(serverpath, path);
			ctx->path = serverpath;
		}
		
		printf("Calling Handler");
		
		gfs->handler(ctx, ctx->path, gfs->handlerarguments);
		
		printf("Cleaning Up\n\n");
		
		//Close socket, move file descriptor back to the beginning of the file, and reset @message_end
		close(new_socket);
		message_end = BUFSIZE;
		serverpath[0] = '\0';
		message[0] = '\0';
	}
    
	return;
}

void gfserver_set_handlerarg(gfserver_t *gfs, void* arg){
	gfs->handlerarguments = arg;
	return;
}

void gfserver_set_handler(gfserver_t *gfs, ssize_t (*handler)(gfcontext_t *, const char *, void*)){
	gfs->handler = handler;
	return;
}

void gfserver_set_maxpending(gfserver_t *gfs, int max_npending){
	gfs->maxpending = max_npending;
	return;
}

void gfserver_set_port(gfserver_t *gfs, unsigned short port){
	gfs->port = port;
	return;
}


