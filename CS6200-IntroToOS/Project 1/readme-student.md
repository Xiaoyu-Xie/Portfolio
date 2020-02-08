# GETFILE Implementation README file

This is a readme file for an implementations of the GETFILE file transfer protocol in both a single-threaded and multithreaded design. Below are the descriptions of the implementation and testing followed by a journal of issues that occurred during the design and suggestions on improvements.

## Project Description
#### 1. Echo Client-Server Warmup
##### Implementation
To start, the connection and initial communication pieces were developed in a separate program that models a basic echo client-server where a client establishes a connection with a listening server using sockets. Once the connection is made the client can send messages to the server which the server will copy and send right back to the client.
- __CLIENT__
    - All source code for the client is written in the file *echoclient.c* and does not utilize any user-defined headers
    - The port (-p), server (-s), and message (-m) can be given via command line arguments. The options are shown in the parentheses and (-h) can be used for a more detailed breakdown of the options
    - The program flow is the following:
        1. Command line arguements are parsed, stored in the proper variables, and checked for validity
        2. The hostname is resolved into an IP using *getaddrinfo*
        3. Socket API from the *<sys/socket.h>* header is used to connect to the server
        4. The *send* and *read* functions are used to send the message to the server and read the response
                a. The response is written to the client console
        5. The socket is closed and the program is exited
- __SERVER__
    - All source code for the client is written in the file *echoserver.c* and does not utilize any user-defined headers
    - The port (-p) and maximum amount of pending responses allowed before new connections will be rejected (-m) can be given via command line arguments. The options are shown in the parentheses and (-h) can be used for a more detailed breakdown of the options
    - The program flow is the following:
        1. Command line arguements are parsed, stored in the proper variables, and checked for validity
        2. Socket API from the *<sys/socket.h>* header is used to create a socket description and bind to a socket for listening
        3. The *read* and *send* functions are used to read the sent message and send a response
        4. The socket is closed and the program is exited
##### Testing
1. Tested that the client compiles
2. Tested that the server compiles
3. Tested that the server echoed the message from the client
4. Tested that the client printed the message sent from the server

#### 2. File Transfer Warmup
##### Implementation
Next, the pieces were developed in a separate program that models a basic file transfer client-server where a client establishes a connection with a listening server using the communication pieces developed in the echo client-server program. Once the connection is made the client receives a file from the server without a request having to be made.
- __CLIENT__
    - All source code for the client is written in the file *transferclient.c* and does not utilize any user-defined headers
    - The port (-p), server (-s), and output file path (-o) can be given via command line arguments. The options are shown in the parentheses and (-h) can be used for a more detailed breakdown of the options
    - The program flow is the following:
        1. Command line arguements are parsed, stored in the proper variables, and checked for validity
        2. The hostname is resolved into an IP using *getaddrinfo*
        3. Socket API from the *<sys/socket.h>* header is used to connect to the server
        4. The output file is created and opened
        5. The file sent from the server is received and read by using the *recv* function
        6. The data read is written from the receiving buffer into the output file
        7. Once, the server closes the connection at the end of the file transfer, The socket is closed and the program is exited
- __SERVER__
    - All source code for the client is written in the file *transferserver.c* and does not utilize any user-defined headers
    - The port (-p) and filename of the file to be transfered (-f) can be given via command line arguments. The options are shown in the parentheses and (-h) can be used for a more detailed breakdown of the options
    - The program flow is the following:
        1. Command line arguements are parsed, stored in the proper variables, and checked for validity
        2. The file to be transfered is opened for reading
        3. Socket API from the *<sys/socket.h>* header is used to create a socket description and bind to a socket for listening
        4. Once a connection from a client has been accepted, the *fread* and *send* functions are used to read data from the file being transfered and send that data to the client
        5. When the end of the file is reached, the socket is closed and the program is exited
##### Testing
1. Tested that the client compiles
2. Tested that the server compiles
3. Tests that the server accurately sends the file to the client
4. Tested that the client accurately saves the file sent from the serve

#### 3. GETFILE Single-Threaded
##### Implementation
Next, the GETFILE protocol was implemented using the client-server connection mechanism designed in the echo client-server program and the file transfer mechanism designed in the transfer client-server program. The GETFILE protocol adds additional functionality and complexity to transfering files as the client first sends a request message of the format *<scheme> <method> <path>\r\n\r\n* where the *scheme* is GETFILE, the *method* is GET to indicate the client is requesting a file, and the *path* is the file path of the file the client is requesting. The string *\r\n\r\n* is a terminator that signals the end of the header information. The server responds with a message containing the following header *<scheme> <status> <length>\r\n\r\n* where the *scheme* is GETFILE, the *status* is either "OK", "INVALID", "FILE_NOT_FOUND", or "ERROR", and the *length* is the length of the file that the client requested. The response message may contain some or all of the file contents. If all of the contents weren't sent in the response message, the server will keep sending messages with the remaining file contents until the file has been completely transferred.
- __CLIENT__
    - The GETFILE Client implementation is written in the files *gfclient.c* and *gfclient.h*
    - __*gfcrequest_t*__: The structure used to hold all information about the request the client will be making. Its fields and methods are the following:
        * Server Connection Fields: 
            - char \**server*
            - unsigned short *port*
        * Receive Statistics: 
            -  size_t *bytes_received*
            -  size_t *filelen*
        * Function Properties:
            - char \**path*
            - gfstatus_t *status*
            - void (\**writefunction*)(void \*, size_t, void \*)
            - void \**writearguments*
            - void (\**headerfunction*)(void \*, size_t, void \*)
            - void \**headerarguments
    - The program flow is the following:
        1. Request Creation: The programmer initializes the request object by calling *gfc_create()* which allocates memory for a new *gfcrquest_t* object, sets the fields to their defaults, and returns a pointer to the new *gfcrequest*
        2. Request Setting: The programmer should set server hostname, path, port, write function (function created by the programmer to write the received file contents to a file), and write argument by calling *gfc_set_server, gfc_set_path, gfc_set_port, gfc_set_writefunc,* and *gfc_set_writearg*, respectively, and passing them the *gfcrequest* and the value to be set for each of them.
            * The write function is a function written by the programmer that handles writing the received data to where the programmer wants it to go
        2. The hostname is resolved into an IP using *getaddrinfo*
        3. Socket API from the *<sys/socket.h>* header is used to connect to the server
        4. Next, the client contructs the request and sends it to the server
        4. The client then listens for the response header by reading until the termination string is seen
            a. During the read of the header, the data coming after the termination string is retained and those bytes are added to the total bytes received counter
        5. The header is then parsed into the scheme, status, and file length
        6. The parsed values are then checked for errors
            a. The status of the request is set to INVALID, the socket close, and a -1 returned for error if any of the following conditions are met:
            - The scheme is not "GETFILE"
            - The status is not "INVALID", "ERROR", "FILE_NOT_FOUND", or "OK"
            - The file length is not found when the status is "OK"
            - The status is "INVALID"
        
            b. If the status is "ERROR" or "FILE", the status of the request is set to the same status, the socket is closed, and 0 is returned
        6. If the status is "OK", the file length is parsed and the contents are passed to the write callback function
        7. The client then reads the messages from the server while recording the bytes from the read operation, adds them to the total bytes counter, and calls the write callback function, passing it the data from the read. 
        a. if 0 or less bytes are received during the read loop, the client closes the socket and returns a -1 to indicate an error
        11. The client exits the reading loop when the total bytes received equals the file length sent by the server, and closes the socket, adds the total bytes read to the gfrequest, sets the status to "OK", and returns 0 to indicate a proper exit.
- __SERVER__
    - The GETFILE Client implementation is written in the files *gfserver.c* and *gfserver.h*
    - __*gfserver_t*__: GETFILE uses this structure to hold all the server listening properties and the function used for handling the request. Its fields and methods are the following:
        * Connection Settings
            - unsigned short *port*
            - int *maxpending*
        * Function Properties
            - ssize_t (\**handler*)(gfcontext_t \*, const char \*, void \*)
            - void \**handlerarguments*
    - __*gfcontext_t*__: GETFILE uses this structure to hold all the request information. Its fields and methods are the following:
        * Request Properties
            - char \**path*
            - int *sock_id*
    - The program flow is the following:
        1. The server object is created by calling *gfserver_create()* which returns a *gfserver_t* pointer with the *port* and *maxpending* set to 12041 and 5 respectively. Meanwhile the *handler* and *handlerarguments* are set to NULL.
        2. The port, maxpending, handler arguements and handler are set by the programmer by calling *gfserver_set_port, gfserver_set_maxpending, gfserver_set_handlerarg* and *gfserver_set_handler* respectively.
        3. Once everything has been created and initialized, the servers main function is started by calling *gfserver_serve* and passing it the pointer for the *gfserver_t* that was created.
        4. First, the variables are delcared and intitialized followed by the creation and binding of the socket in the same manner as the previous programs.
        5. The server then begins listening for new connections.
        6. Once a new connection is made, the accepted socket ID is stored in a *gfcontext_t* object.
        7. The server then reads all messages from the client until the termination string is seen.
        8. After the termination string is seen, the request from the client is parsed.
        a. An "INVALID" response is sent to the client by calling *gfs_sendheader* with "INVALID" passed to it if any of the following conditions occur:
            * The scheme is not "GETFILE"
            * The method is not "GET"
            * The path does not start with "/"
        9. The path from the client request is added to the *gfcontext_t* object and the object, the path, and the handlerarguments are passed to the server handler.
        10. This handler then calls *gfs_sendheader* to generate a response.
        11. *gfs_sendheader* creates a header response to send to the client using a *gfcontext_t*, status, and file lenth that ar passed to it.
        12. If the status is "OK", the file length is included in the header.
        13. Once the header is constructed, it is sent to the client using the socket in the *gfcontext_t* object.
        14. After the header has been sent, if the status was "OK", the handler will eventually call the function *gfs_send*.
        15. *gfs_send* sends the data passed to it by the handler and is assumed to be the contents of the file.
        16. To start, the sending function figures how many chunks it needs to send in order to send all the data passed to it with the size buffer that it uses as well as the size of the last chunk that will be sent.
        17. Then the data is sent by being copied into the buffer and using *send* with the socket ID from the *gfcontext_t* object that was passed to the function
        a. If send returns a value less than 0, the socket is closed and -1 is returned to indicate an error.
        18. The handler will call *gfs_send* until all the data is sent. Then it will return to the function *gfserver_serve*.
        19. The serving function will then close the socket, reinitialize the used variables and loop back to the listening step.
##### Testing
1. Tested that the client compiles
2. Tested that the server compiles
3. Tested that the client handles a prematurely closed connection during the transfer of the file
4. Tested that the client handles a prematurely closed connection during the transfer of the response header
5. Tested that the server handles a prematurely closed connection during the transfer of the file
6. Tested that the server handles a prematurely closed connection during the transfer of the request header
7. Tested that the client handles an ERROR response from the server
8. Tested that the client handles a FILE_NOT_FOUND reponse from the server
9. Tested that the client handles an INVALID response from the server
10. Tested that the client properly handles an OK response and a long message (more than 5 MB) sent in varying size chunks between 64 bytes and 64KB
11. Tests that the client properly handles an OK response and a short message (less than 1000 bytes)
12. Tests to ensure client can properly handle a file with control data

#### 4. GETFILE Multithreaded
##### Implementation
The GETFILE protocol described in the program above was implemented using the same definitions for the protocol but this time, server and client programs were written to use the GETFILE libraries in a multithreaded fashion. This was done using the "Boss-Worker Model" where for the client, the main thread takes in files to download from a text file and places the paths in a queue for the worker threads to retrieve to start their own GETFILE transfer sessions. The server application uses the main thread to take in requests from clients and puts them into a queue for worker threads to respond to and send the file requested. The workers return to the queue after finishing. If there are more jobs, they pick one of the queue. If not, they wait till the main thread has put more on the queue.

- __CLIENT__
    - The GETFILE Multithreaded Client implementation is written in the files *gfclient_download.c* and *gfclient-student.h*
    - __*request_t*__: The multithreaded client uses this structure to hold all of the request information that the worker threads needs to use to initiate a transfer from the server. Its fields and methods are the following:
        * gfcrequest_t \**gfr*
        * char \**path*
        * FILE \**file*
    - The global variables are used for data and objects that all threads need to be able to access and are the following:
        * steque_t *work_queue* (This is a queue that comes from the source file *steque.c* that was provided)
        * pthread_t \**threads*
        * pthread_mutex_t *work_queue_lock*
        * pthread_mutex_t *finished_lock*
        * pthread_cond_t *queue_not_empty*
        * int *finished*
    - The program flow is the following:
        1. The variables are delcared and initialized.
        2. The command line arguments are parsed for the number of requests (-n), port (-p), server name (-s), number of threads (-t), and the path of the text file containing the file requests, called the workload path (w).
        3. The workload file is opened and accessed using a source file *workload.c* that was provided.
        4. The multithreaded variables and worker threads are created and inititalized in *multithreadSetup(number of threads)*.
            a. In *multithreadSetup()* the pthread array is initialized and the worker threads are told to start executing the function *workerReceiveFiles()*.
            b. Two mutex locks and one condition variable are created and initialized.
            - *work_queue_lock* and *finished_lock* control access to the request queue and a variable that signals to the workers that there are no requests left respectively. 
            - *queue_not_empty* is a condition variable that the main thread signals to let workers that are waiting on an empty work queue know that more requests have been added.
        5. The main thread then enters for loop that ends after all requests in the workload file have been made and queued.
        a. In this loop, the main thread (Boss thread) gets a file path from the workload API and creates a new *request_t* object using *createRequest()* which allocates space for a request object and returns a pointer to the space.
        b. The path from workload is then used to generate the local path on the client where the received file is to be stored by using the *localPath* function that was provided.
        c. The receiving file is then opened and the workload path as well as the file stream are added to the request object.
        d. a *gfrequest_t* object is then created and the server, path, port, write function, and write argument are set using the GETFILE API provided.
            * For this implementation, the write argument is the file stream of the receiving file that was opened earlier.
        
            e. The *gfrequest_t* is then added to the *request_t* object.
            f. The main thread then obtains the lock on the work queue when it is available and if the thread is not empty, it just adds the request to the queue. Otherwise, it adds the request and signals to threads waiting on the condition *queue_not_empty* using broadcast that the thread now has requests.
            g. The main thread then unlocks the work queue lock.
        6. Once all requests have been added, the main thread exits the loop, locks the finished lock, changes *finished* from a 0 to a 1, unlocks the funished lock, and broadcasts the *queue_not_empty* condition one more time to release any workers that may still be waiting on a request.
        7. Lastly, *pthread_exit* is called so that the main thread will wait on the workers to finish before ending and returns 0 when everything has ended.
    - The client worker flow is the following:
        1. Once created, the workers begin executing *workerReceiveFiles* which starts by entering a never-ending for loop.
        2. The worker will then obtain the work queue lock and check to see if the work queue is empty while entering a while loop.
        3. If the queue is empty, the worker will obtain the finished lock and check the *finished* variable to see the value is a 0 or 1.
        4. If the finished value is a 1 the worker will unlock the work queue and finished locks and call *pthread_exit* to exit because the main. thread has indicated that there are no more requests to be added to the work queue.
        5. If the finished value is a 0 the worker will unlock the finished lock and waitd on the *queue_not_empty* condition that is signaled by the main thread.
            * When the main thread signals that the queue is not empty, there may be multiple threads trying to get a few requests. Since waiting workers are released one at a time, each worker loops back to checking if the queue is empty.
        6. Once the queue is not empty when a worker checks the empty status, it pops the next request off the queue unlocks the work queue.
        7. The *gfrequest_t* in the request object is then passed to *gfc_perfrom* and the worker sends the file to the requesting client
        8. Once done, *gfc_cleanup* is called and passed the *gfrequest_t object from the request for cleanup.
        9. Lastly, the worker frees the request object and continues the never-ending for loop

- __SERVER__
    - The GETFILE Multithreaded Server implementation is written in the files *gfserver_main.c, gfserver-student.h*, and *handler.c*
    - __*request_t*__: The multithreaded server uses this structure to hold all of the request information that the worker threads needs to use to initiate a transfer from the server. Its fields and methods are the following:
        - gfcontext_t \**ctx*
        - const char \**path*
    - The program flow is the following
        1. The variables are delcared and initialized.
        2. The command line arguments are parsed for port (-p), number of threads (-t), and the content map (-m).
        3. The gfserver is created by calling *gfserver_create()* which returns a pointer to a *gfserver_t* object.
        4. The port, maximum pending requests, handler, and handler arguments are set using the same functions from the previous program.
        5. The function *createWorkers* is then called with the number of threads passed to it so it can create and initialize threads.
            - *createWorkers* is written in the *handler.c* file and contains the following global variables for all threads to see:
                - int *number_of_threads*
                - steque_t *work_queue*
                - pthread_t \**threads*
                - pthread_mutex_t *work_queue_lock*
                - pthread_cond_t *queue_not_empty*
            - *createWorkers* sets the global number of threads variable with the value that was passed to it and then callse *multithreadSetup* which is located in the same source file and initializes the mutex lock, condition variable, and creates the threads, giving them *workerSendFiles* to start executing.
        6. After creating the threads, the main thread will call *gfserver_server* from the GETFILE library and pass it the gfserver created earlier.
        7. While running, the server will call the handler given to the gfserver object earlier when a request is received which is written in *handler.c*.
        8. this handler, *gfs_handler*, will create a new *request_t* object and give it the *gfcontext_t* object and path that were passed to the handler.
        9. The server then obtains the work queue lock and pushes the request onto the queue.
        10. If the queue is empty, it will also send a broadcast signal on the *queue_not_empty* condition.
        11. Next the server unlocks the work queue and returns 0 to indicate a success.
        12. This continues until the server is terminated.
    - The server worker flow is the following:
        1. Once a worker thread is created, it begins executing the function *workerSendFiles* that is written in *handler.c*.
        2. The worker will enter a never-ending for loop that starts with obtaining the lock on the work queue.
        3. Once the lock is obtained, the worker will enter a while loop while the work queue is empty.
        4. If the work queue is empty, it will wait on the *queue_not_empty* condition.
        5. Once the condition is broadcast by the main thread, the worker will evalutate if the queue is empty when it is released.
        6. If it is, it will wait for the next broadcast.
        7. If it isn't empty, it will pop off the next request and unlock the work queue. The same thing it does if the queue is not empty the first time it checks.
        8. Next the file descriptor for the file that needs to be sent is retrieved by calling *content_get* and passing it the path from the request. This utilizes the content API that was provided.
        9. If the returned descriptor is -1, that means the file cannot be found and the worker will call *gfs_sendheader* to send a file not found response and then start the never-ending for loop over again.
        10. If a valid descriptor is returned, the the file length is calculated.
        11. A response header is then sent to the requesting client by passing the gfcontext from the request, an OK status, and the file length to *gfs_sendheader*.
        12. The worker then starts sending the file by entering a while loop that loops until the total bytes sent counter is the same value as the file length.
        13. The worker reads bytes from the file into its buffer and sends them by passing the gfcontext, data buffer, and the bytes read returned by the read to *gfs_send*
            a. If the bytes read is less than or equal to 0, the worker exits the while loop and continues on.
        14. Once the send is finished, the request is freed, the total bytes read counter is reset, and the work loops back to the start of the never ending loop.

##### Testing
1. Tested that the client compiles
2. Tested that the server compiles
3. Ensure server can support files larger than available memory
4. Verify parallel thread activity serving files under 2KB
5. Multiple clients downloading mixed files simultaneously
6. Simultaneous downloads of one file under 2KB in size
7. Simultaneous downloads of files sized between 1KB to 1MB
8. Simultaneous downloads of files up to 2KB in size
9. Tested that the client handles an INVALID response from the server
10. Tested that the client properly handles an OK response and a long message (more than 5 MB) sent in varying size chunks between 64 bytes and 64KB
11. Tests that the client properly handles an OK response and a short message (less than 1000 bytes)
12. Tests to ensure client can properly handle a file with control data
13. Tests that the client uses multiple threads while downloading small-sized files
14. Tests that the client uses multiple threads while downloading 1MB files
15. Test for boss worker model using pthreads
16. Tests that the implementation is using resources appropriately
17. Tests to ensure initialization and cleanup are done properly
18. Test to ensure client can download a large file
19. Tests that the client simultaneously downloads multiple files in a timely manner

## Issues & Improvements
    - The biggest challenge in multithreading was figuring out how to handle multiple threads waiting on an empty queue
        - Solved by using a broadcast signal instead of the normal signal and then having wakened threads check to see if the queue is empty again before doing anything else
    - The biggest challenge in the single threaded setup was with my client failing numerous tests and hanging up.
        - I solved this by reanalyzing every possible loop, read, and send to make sure they handled all scenarios. Then I had to shrink the size of my read and send buffers
    - If I were to do this again, I'd utilize the student headers for the single threaded implementation the same way I did for the multithreaded setup. It would look a lot cleaner and it would be easier to make adjustments and troubleshoot
    - The only thing I didn't find clear in the documentation was not being sure whether my implementations needed to return -1 or 0 when FILE_NOT_FOUND, INVALID or ERROR occurred

## Known Bugs/Issues/Limitations
    1. The Bonnie tests for gfserver_mt did not seem to accurately count the threads in my implementation and failed my code on the test where it counts those
    2. My multithreaded client fails the test on being able to handle a download larger than the available memory
    
## References
    1. https://beej.us/
        - Socket connection tutorials
    2. https://stackoverflow.com/
        - Used for reminders like how to use *fstat*


