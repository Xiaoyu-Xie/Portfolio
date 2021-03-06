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
#include <arpa/inet.h>

#define BUFSIZE 1033

#define USAGE                                                                 \
"usage:\n"                                                                    \
"  echoserver [options]\n"                                                    \
"options:\n"                                                                  \
"  -p                  Port (Default: 12041)\n"                                \
"  -m                  Maximum pending connections (default: 1)\n"            \
"  -h                  Show this help message\n"                              \

/* OPTIONS DESCRIPTOR ====================================================== */
static struct option gLongOptions[] = {
  {"port",          required_argument,      NULL,           'p'},
  {"maxnpending",   required_argument,      NULL,           'm'},
  {"help",          no_argument,            NULL,           'h'},
  {NULL,            0,                      NULL,             0}
};


int main(int argc, char **argv) {
  int option_char;
  int portno = 12041; /* port to listen on */
  int maxnpending = 1;
  
  // Parse and set command line arguments
  while ((option_char = getopt_long(argc, argv, "p:m:hx", gLongOptions, NULL)) != -1) {
   switch (option_char) {
      case 'p': // listen-port
        portno = atoi(optarg);
        break;                                        
      default:
        fprintf(stderr, "%s ", USAGE);
        exit(1);
      case 'm': // server
        maxnpending = atoi(optarg);
        break; 
      case 'h': // help
        fprintf(stdout, "%s ", USAGE);
        exit(0);
        break;
    }
  }

    setbuf(stdout, NULL); // disable buffering

    if ((portno < 1025) || (portno > 65535)) {
        fprintf(stderr, "%s @ %d: invalid port number (%d)\n", __FILE__, __LINE__, portno);
        exit(1);
    }
    if (maxnpending < 1) {
        fprintf(stderr, "%s @ %d: invalid pending count (%d)\n", __FILE__, __LINE__, maxnpending);
        exit(1);
    }

    struct sockaddr_in listen_addr;

  /* Socket Code Here */
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(portno);
	listen_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//inet_pton(AF_INET, "127.0.0.1", &listen_addr.sin_addr);

    int new_socket, clientlen, one = 1;
    char buffer[BUFSIZE] = {0};
    int sock_id = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(sock_id, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));
    bind(sock_id, (struct sockaddr *) &listen_addr, sizeof(listen_addr));

    while(1) {
		listen(sock_id, maxnpending);
		clientlen = sizeof(struct sockaddr_in);
		new_socket = accept(sock_id, (struct sockaddr *) &listen_addr, (socklen_t*) &clientlen);
	
	
        read(new_socket, buffer, BUFSIZE);
		printf("%s", buffer);
        send(new_socket, buffer, strlen(buffer), 0);
		
		for(int i = 0; i < 16; i++) {
			buffer[i] = 0;
		}
	}
    
	return 0;

}
