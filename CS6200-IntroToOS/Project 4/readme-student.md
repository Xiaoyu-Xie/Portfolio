# Project 4 README file

## Project Description

This purpose of this project is to implement service that reduces the resolution jpg images via RPC in both a single and multithreaded configuration

### Implementation
##### Single Threaded
The first step was to created the RPC structures by designing the file "minifyjpeg.x". This contains 2 structure: "minify_in" and "minify_out" which each have an opaque, variable-length field designed to hold data for the original image and the lower resolution result, respectively. They are used in the RPC function "minify_out MINIFY_WITH_RPC(minify_in)". 

Once the RPC structure was designed, the server-side functions were created in "minifyjpeg.c". The main service function, "minify_with_rpc_1_svc", takes in the minify_in object from the request and passes the original image data to the magickminify function "magickminify". The resulting pointer is then given to the minify_out object from the request. The function then returns. Later on in the process "minify_rpc_1_freeresult" is call which is also implemented in "minifyjpeg.c" where the xdr object for the result is freed.

On the client side, the primary client program "minifyjpeg_main.c" makes calls to two functions that are specified in "minify_via_rpc.c". In the function "get_minify_client", the "CLIENT" object for the RPC communication is created by calling "clnt_create" and the time out is set to 3 seconds using "clnt_control". Later on, the client worker threads call "minify_via_rpc" which receives the "CLIENT" object from the worker thread as well as pointers to where the original image data is held, where the resulting image data should be stored, and where the size of the resulting image should be stored. It begins by creating the "minify_in" and "minify_out" objects for the RPC call as well as the "clnt_stat" object for storing the status message of the RPC call. After memory has been allocated for the results and the input has been initialized, the two objects are passed to the function "minify_with_rpc_1" which was created through "rpcgen" after the "minifyjpeg.x" was designed. If the result from the call is "RPC_TIMEDOUT", "minify_via_rpc" returns that value, otherwise, the results are placed in the pointers for the resulting length and data that were passed the function from the worker thread and the value 0 is returned.

##### Multithreaded

Since the client side was already multithreaded, the server was made to be multithreaded by editing the file "minifyjpeg_svc.c" by "rpcgen". A request queue, thread pool, and mutexes were added and initialized in the function "main". In the handler function "minify_rpc_1", which is called by the server service when a request is received, it was modified to place the request on a shared queue. The request is stored in a "request_t" object which contains the "minify_in" object received from the client, the "SVCXPRT" object used to communicate with the client, and the "svc_req" object. The worker threads that were initialized in the "main" function execute the function "requestHandler" that begins by checking the request queue for requests. Once a request is placed on the queue, the handling thread wakes the waiting threads using a pthread condition broadcast. The thread that gets the request will then call the function "minify_with_rpc_1" which is implemented in "minifyjpeg.c". The worker thread will then send the reply to the client, free the memory used by the request, and begin checking the queue again.


### Issues, Testing, and Improvements

- During the single threaded implmentation, I had issues because I named the RPC process "minify_via_rpc" which caused there to be 2 functions of the same name on the client side. Luckily some errors pointed me in the right direction
- Also during the single threaded implementation, I designed my RPC input and output structures incorrectly. After getting consistent segfaults, I reread the code better saw the purposes and expectations of the values passed to "minify_via_rpc" and how to use them.
- During the multithreaded implementation, I did not have any issues. I used code I had written in project 3 with some modifications and passed the Bonnie tests on the first try.
- The documentation this time was very clear and the resources given were enough answer all of my questions about implementation.
- As for additional tests, I think there are some different "clnt_stat" codes that could have been tested for and we could have been made to handle.
- What I would do to improve my project is the same as always: more error checking/handling and meaningful error messages for troubleshooting.

## Known Bugs/Issues/Limitations

No known issues.

## References

Mainly used the Oracle documentation linked in the project document and miscellaneous questions I had were looked up on Stack Overflow.

