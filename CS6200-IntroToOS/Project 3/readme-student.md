
# Project 3: IPC README file

## Project Description
The purpose of this project is to implement a proxy file transfer server that utilizes a smart cache for performance improvements. The proxy server uses the GETFILE library when communicating with the client and the Libcurl library (specifically the HTTP "easy" implementation) when communication with the file server.

## Implementation
### Part 1 - Proxy Server Only
This implementation of the proxy server begins by registering the handle "handle_with_curl" in the GETFILE server. This handler is called when requests are received from the client and performs the following steps:

1. Request the HTTP header for the file to check for errors and to get the file size
    - This is done using the function "getFileSize" defined in the header file "proxy-student.h". This function sets the curl handle options to only have the header file sent over, then it determines the file size from the "Content Length" field. If there are errors, error values are returned. If there are no errors, the file size is returned. 
2. Reply to client
    - This is done in "handle_with_curl". If "getFileSize" returned a -1, an error occurred and the client is sent a response with the GF_ERROR status and "handle_with_curl" returns a -1. If -2 is returned, the file was not found and the client is sent a response with the GF_FILE_NOT_FOUND status and "handle_with_curl" returns a -1. For all other return values, the file size is sent in the response to the client with the GF_OK status.
3. Transfer the File
    - This is done in the function "transferFile". It is called from "handle_with_curl" if the file size was able to be determined without error. This function sets the easy curl options so that the body and no header for the file are returned. It also sets the write function for the curl handle to be the function "write_data". Once the options have been set, the easy curl is performed which will call "write_data". In "write_data", "gfs_send" is called using the correct GETFILE context until all of the data in the "buffer" is sent to the client. Supports chunking during the "gfs_send" loop. The correct context is managed by using a custom structure called "writeinfo_t" that holds a gfcontext and url. It is created in "handle_with_curl" and given to "transferFile" to hand to the curl handle for use in the write function, "write_data".

### Part 2 - Smart Cache
The implementation of the proxy used the same GETFILE implementation as in part 1, but this time the HTTP file send was replaced with shared memory being used to get the requested file from another process on the same machine. The proxy receives the GETFILE request the same as before but the handle "handle_with_curl" checks with the cache process to see if it has the file via a worker thread. 

1. The proxy does this by sending a message of the created type "request_t" on the requests message queue that was created by the cache and waits for the response.
2. When a request is received, the cache boss/listener takes in the request, adds it to the requests queue, and notifies the worker threads that a request is available.
3. The cache worker thread that gets the request uses the simple cache to see if the file is there and sends a response using the command message queue for sending messages from cache to proxy.
    - This message queue is created by the proxy and the id is shared with the cache via the request message.
4. If the file was found, the worker thread will populate the shared memory segment that the request message said to use with the first set of data from the file.
    - If the file was not found or there was an error, the cache worker will have let the proxy worker know in its response message and then exit.
5. The cache worker will then let the proxy worker know that the data is ready via the cache to proxy message queue.
6. The proxy worker will then take the data in the shared memory and send it to the GETFILE client.
7. Once finished, the proxy worker will notify the cache worker via the proxy to cache message queue
    - This message queue is created by the proxy and the id is shared with the cache via the request message
8. Once the cache worker and proxy worker have calculated that all of the data has been sent, the cache worker goes back to checking on the request queue and the proxy worker thread exits "handle_with_curl" returning the bytes sent.
    - The proxy is sent the file size by the cache worker in the response message from step 3

## Problems, Tests, and Improvements
### Part 1

- I had a hang up with figuring out the file size prior do the download in order to respond to the client, but after having the HTTP header print to the console, and looking at the values for different scenarios, I found a system that works.
- I also struggled with Bonnie at first with getting the generic "something didn't happen right" error on transfers, but I consulted Piazza and found it was an issue with my URL.
- If I were to improve my design, I would do more error checking and create better error messages for security and troubleshooting sake.
- I thought the documentation was clear but I continue to be frustrated with Bonnie. Since I can't really debug with Bonnie (sometimes there's not proxy console output, I can't control the return codes, etc.), I find myself having to submit code that I know is wrong just to try to get something different in the results to point me in the right direction. Because of that, having a 10 submission limit per 24 hours is a massive pain.

### Part 2

- At first I had too many functions in "proxy-student.h" which is used by both processes. This caused a lot of unexpected compilation issues. I fixed it by having only structures and enums in the "proxy-student.h" file
- Later I had a lot of issues with requests writing data to the wrong requests because of shared memory ids getting the same id. This was because I needed to dynamically create unique files for their keys to use.
- If I were to improve my design, I would do more error checking and create better error messages for security and troubleshooting sake. This is always the thing I leave out when I'm running up on the deadline.
- The documentation was pretty clear, but a better explanation of System V IPCs would have been nice. Or a link to a reliable website. I found a site, but I found later that it told me to use the wrong value for an argument in shmget.

## Known Bugs/Issues/Limitations

 - Passed all tests for both parts

## References

    - https://www.softprayog.in/programming/interprocess-communication-using-system-v-shared-memory-in-linux
    - https://computing.llnl.gov/tutorials/pthreads/#Mutexes
    