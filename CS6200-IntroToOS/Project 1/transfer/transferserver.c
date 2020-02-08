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
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <fcntl.h>

#define BUFSIZE 1033

#define USAGE                                                \
    "usage:\n"                                               \
    "  transferserver [options]\n"                           \
    "options:\n"                                             \
    "  -f                  Filename (Default: 6200.txt)\n" \
    "  -h                  Show this help message\n"         \
    "  -p                  Port (Default: 12041)\n"

/* OPTIONS DESCRIPTOR ====================================================== */
static struct option gLongOptions[] = {
    {"filename", required_argument, NULL, 'f'},
    {"help", no_argument, NULL, 'h'},
    {"port", required_argument, NULL, 'p'},
    {NULL, 0, NULL, 0}};

int main(int argc, char **argv)
{
    int option_char;
    int portno = 12041;             /* port to listen on */
    char *filename = "6200.txt"; /* file to transfer */

    setbuf(stdout, NULL); // disable buffering

    // Parse and set command line arguments
    while ((option_char = getopt_long(argc, argv, "p:hf:x", gLongOptions, NULL)) != -1)
    {
        switch (option_char)
        {
        case 'p': // listen-port
            portno = atoi(optarg);
            break;
        default:
            fprintf(stderr, "%s", USAGE);
            exit(1);
        case 'h': // help
            fprintf(stdout, "%s", USAGE);
            exit(0);
            break;
        case 'f': // listen-port
            filename = optarg;
            break;
        }
    }


    if ((portno < 1025) || (portno > 65535))
    {
        fprintf(stderr, "%s @ %d: invalid port number (%d)\n", __FILE__, __LINE__, portno);
        exit(1);
    }
    
    if (NULL == filename)
    {
        fprintf(stderr, "%s @ %d: invalid filename\n", __FILE__, __LINE__);
        exit(1);
    }
	
	/*
	* Declare and initialize variables
	*/
	FILE *input;									//File descriptor for the file that is being transferred
	int message_end = BUFSIZE;						//Holds the amount of bytes read for each fread
    int new_socket;									//Socket descriptor for the socket used once the connection has been accepted
	int clientlen = sizeof(struct sockaddr_in);		//Size is needed to accept a connection
	int one = 1;									//The socket options require the address of an interger but for our purposes, it is not important
    int sock_id = socket(AF_INET, SOCK_STREAM, 0);	//Socket descriptor for the listening socket that is bound to the IP
	char message[BUFSIZE] = {0};					//Byte buffer for holding the message that is sent to the client
	struct sockaddr_in listen_addr;					//Socket Address structure for holding the properties and settings of the listening socket

	//Specifies that the listening port will use IPv4, and what port and IP it will use
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(portno);
	listen_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	//Sets the options for the listening socket. Most importantly, that the ip address can be resused for simultaneous connections
	setsockopt(sock_id, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));
	
	//Binds a socket to the descriptor
    bind(sock_id, (struct sockaddr *) &listen_addr, sizeof(listen_addr));
	
	//Open file that is being transferred
	input = fopen(filename, "rb");

	/*
	* Listen for connections then send the file in buffer size messages
	*/
    while(1) {
		//Begin listening
		listen(sock_id, 10);
		
		//Accept connection and assign it to a new socket descriptor
		new_socket = accept(sock_id, (struct sockaddr *) &listen_addr, (socklen_t*) &clientlen);
	
		//Continue reading from the file until it returns less than the buffer size, indicating the end of the file
		while (message_end == BUFSIZE)
		{
			//Read message
			message_end = fread(message, 1, BUFSIZE, input);
			
			//Add string terminator, send message, and clear the buffer
			if(message_end < BUFSIZE)
				message[message_end] = '\0';
			send(new_socket, message, message_end, 0);
			message[0] = '\0';
		}
		
		//Close socket, move file descriptor back to the beginning of the file, and reset @message_end
		close(new_socket);
		fseek(input, 0, SEEK_SET);
		message_end = BUFSIZE;

	}
    
	return 0;

}
