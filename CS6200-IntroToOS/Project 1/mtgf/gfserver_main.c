#include <getopt.h>
#include <stdio.h>
#include <sys/signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>

#include "gfserver.h"
#include "content.h"

#include "gfserver-student.h"

#define USAGE                                                                 \
"usage:\n"                                                                    \
"  gfserver_main [options]\n"                                                 \
"options:\n"                                                                  \
"  -t [nthreads]       Number of threads (Default: 64)\n"                      \
"  -p [listen_port]    Listen port (Default: 12041)\n"                         \
"  -m [content_file]   Content file mapping keys to content files\n"          \
"  -h                  Show this help message.\n"                             \

/* OPTIONS DESCRIPTOR ====================================================== */
static struct option gLongOptions[] = {
  {"port",          required_argument,      NULL,           'p'},
  {"nthreads",      required_argument,      NULL,           't'},
  {"content",       required_argument,      NULL,           'm'},
  {"help",          no_argument,            NULL,           'h'},
  {NULL,            0,                      NULL,             0}
};


extern ssize_t gfs_handler(gfcontext_t *ctx, const char *path, void* arg);

static void _sig_handler(int signo){
  if ((SIGINT == signo) || (SIGTERM == signo)) {
    exit(signo);
  }
}

/* Main ========================================================= */

/*
* 1. Declare and initialize variables
*/
int main(int argc, char **argv) {
  int option_char = 0;
  int nthreads = 64;
  unsigned short port = 12041;
  char *content_map = "content.txt";
  gfserver_t *gfs = NULL;

  //Clear console
  setbuf(stdout, NULL);

/*
* 2. Error Handling for Signal Interrupt and Signal Termination
*/
  if (SIG_ERR == signal(SIGINT, _sig_handler)){
    fprintf(stderr,"Can't catch SIGINT...exiting.\n");
    exit(EXIT_FAILURE);
  }

  if (SIG_ERR == signal(SIGTERM, _sig_handler)){
    fprintf(stderr,"Can't catch SIGTERM...exiting.\n");
    exit(EXIT_FAILURE);
  }

/*
* 3. Parse command line arguments
*/
  // Parse and set command line arguments
  while ((option_char = getopt_long(argc, argv, "t:m:xp:h", gLongOptions, NULL)) != -1) {
    switch (option_char) {
      case 'p': // listen-port
        port = atoi(optarg);
        break;
      case 't': // nthreads
        nthreads = atoi(optarg);
        break;
      case 'h': // help
        fprintf(stdout, "%s", USAGE);
        exit(0);
        break;       
      default:
        fprintf(stderr, "%s", USAGE);
        exit(1);
      case 'm': // file-path
        content_map = optarg;
        break;                                          
    }
  }

  /* not useful, but it ensures the initial code builds without warnings */
  if (nthreads < 1) {
    nthreads = 1;
  }
  
/*
* 4. Create server and initialize settings (port, pending, handler)
*/
  /*Initialize file path mappings*/
  content_init(content_map);

  /*Initializing server*/
  gfs = gfserver_create();

  /*Setting options*/
  gfserver_set_port(gfs, port);
  gfserver_set_maxpending(gfs, 16);
  gfserver_set_handler(gfs, gfs_handler);
  gfserver_set_handlerarg(gfs, (void *) &nthreads); // doesn't have to be NULL!

  createWorkers(nthreads);
  
  /*Loops forever*/
  gfserver_serve(gfs);
  
  pthread_exit(NULL);
}
