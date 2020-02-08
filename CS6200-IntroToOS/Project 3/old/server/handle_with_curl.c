#include <curl/curl.h>

#include "gfserver.h"
#include "proxy-student.h"

#define BUFSIZE (8803)

/*
 * Replace with an implementation of handle_with_curl and any other
 * functions you may need.
 */
ssize_t handle_with_curl(gfcontext_t *ctx, const char *path, void* arg){
/*
* Declare and Initialize Variables 
*/
	char buffer[BUFSIZE];			//Holds the complete URL
	char *data_dir = arg;			//Holds the hostname for the URL
	
	printf("HANDLE: ARG: %s\n", data_dir);
	
	//Libcurl variables
	CURL *curl_handle;				//Libcurl handle for transferring files
	int content_length;				//Holds the size of the file to be transferred
	
	struct writeinfo_t write_info;	//Used to transfer the gfcontext and url between this function and the write function used in curl_easy_perform
	
	//Create URL with host and path
	strncpy(buffer,data_dir, BUFSIZE);
	strncat(buffer,path, BUFSIZE);

	//Initialize write info
	write_info.url = buffer;
	write_info.ctx = ctx;
	
	printf("HANDLE: URL: %s\n", write_info.url);
	
/*	
* Determine file length and send response header
*/
	printf("HANDLE: Initializing LibCurl\n");
	
	//Initialize LibCurl
	curl_handle = curl_easy_init();
	
	//Get file size from header using curl_easy_perform
	content_length = getFileSize(curl_handle, buffer);
	
	//If getFileSize returns -2, the file wasn't found
	if(content_length == -2)
	{
		gfs_sendheader(ctx, GF_FILE_NOT_FOUND, 0);
		return -1;
	}
	//If getFileSize returns -1, curl_easy_peform returned an error
	else if(content_length == -1)
	{
		gfs_sendheader(ctx, GF_ERROR, 0);
		return -1;
	}
	//If getFileSize returns anything else, the file size was found
	else
	{
		gfs_sendheader(ctx, GF_OK, content_length);
	}

/*
* Transfer File
*/
	
	printf("HANDLE: Performing Easy Curl\n");
	
	//The file length was determined, therefore the file can be sent
	transferFile(curl_handle, write_info);
	
	printf("HANDLE: Finishing Handle\n\n");
	
	return 0;
}


/*
 * We provide a dummy version of handle_with_file that invokes handle_with_curl
 * as a convenience for linking.  We recommend you simply modify the proxy to
 * call handle_with_curl directly.
 */
ssize_t handle_with_file(gfcontext_t *ctx, const char *path, void* arg){
	return handle_with_curl(ctx, path, arg);
}	
