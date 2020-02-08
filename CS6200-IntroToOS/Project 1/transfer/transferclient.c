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

#define BUFSIZE 1033

#define USAGE                                                \
    "usage:\n"                                               \
    "  transferclient [options]\n"                           \
    "options:\n"                                             \
    "  -s                  Server (Default: localhost)\n"    \
    "  -p                  Port (Default: 12041)\n"           \
    "  -o                  Output file (Default cs6200.txt)\n" \
    "  -h                  Show this help message\n"

/* OPTIONS DESCRIPTOR ====================================================== */
static struct option gLongOptions[] = {
    {"server", required_argument, NULL, 's'},
    {"port", required_argument, NULL, 'p'},
    {"output", required_argument, NULL, 'o'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0}};

/* Main ========================================================= */
int main(int argc, char **argv)
{
    int option_char = 0;
    char *hostname = "localhost";
    unsigned short portno = 12041;
    char *filename = "cs6200.txt";

    setbuf(stdout, NULL);

    // Parse and set command line arguments
    while ((option_char = getopt_long(argc, argv, "s:p:o:hx", gLongOptions, NULL)) != -1)
    {
        switch (option_char)
        {
        case 's': // server
            hostname = optarg;
            break;
        case 'p': // listen-port
            portno = atoi(optarg);
            break;
        default:
            fprintf(stderr, "%s", USAGE);
            exit(1);
        case 'o': // filename
            filename = optarg;
            break;
        case 'h': // help
            fprintf(stdout, "%s", USAGE);
            exit(0);
            break;
        }
    }

    if (NULL == hostname)
    {
        fprintf(stderr, "%s @ %d: invalid host name\n", __FILE__, __LINE__);
        exit(1);
    }

    if (NULL == filename)
    {
        fprintf(stderr, "%s @ %d: invalid filename\n", __FILE__, __LINE__);
        exit(1);
    }

    if ((portno < 1025) || (portno > 65535))
    {
        fprintf(stderr, "%s @ %d: invalid port number (%d)\n", __FILE__, __LINE__, portno);
        exit(1);
    }

    /*
	* Declare and initialize Variables
	*/
	struct addrinfo hints;		//Server connection properties
	struct addrinfo *srvinfo;	//Server connection information: contains properties, ip, and port
	char port[10] = {0};		//Listening port on remote server
	char buffer[BUFSIZE] = {0};	//Stores bytes received from recv commands
	int bytes_received = 0;		//Bytes received from each recv command
	FILE *output;				//File descriptor for the result of the transfer
	
	//convert @portno into integer and store in @port
	sprintf(port, "%d", portno);
	
	//Reserve memory for @hints
    memset(&hints, 0, sizeof(hints));
  
	//Initialize @hints to use IPv4, TCP Stream, and passive flag
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
  
	//Retrieve IP address for @hostname and store all information in @srvinfo
    getaddrinfo(hostname, port, &hints, &srvinfo);

    //Create socket and connect to remote server based on information from @srvinfo
    int sock_id = socket(srvinfo->ai_family, srvinfo->ai_socktype, srvinfo->ai_protocol);
	connect(sock_id, srvinfo->ai_addr, srvinfo->ai_addrlen);
	
	//Clean up
	freeaddrinfo(srvinfo);
	
	//Open output file
	output = fopen(filename, "w");
	
	//IF output file can't be opened, print error and exit
	if(output == NULL)
	{
		fprintf(stderr, "\nCan't open/create file\n");
		exit(1);
	}
	
	/*
	* Continually read messages from server until recv returns no bytes. Write messages to @output.
	*/
	for (;;) 
	{
		//Read message from server
		bytes_received = recv(sock_id, buffer, sizeof(buffer),0);
		
		//If the bytes received is less than the size of the buffer, add the string termination character to the end of the received message
		if(bytes_received < BUFSIZE)
		{
			buffer[bytes_received] = '\0';
		}
		
		//Exit when the server is finished transferring data and close @output
		if(bytes_received <= 0)
		{
			fclose(output);
			break;
		}
		
		//Write message to @output and prepare for next message
		fprintf(output, "%s", buffer);
		buffer[0] = '\0';
		bytes_received = 0;
	}
	
	//Exit Program
	return 0;
	
}
