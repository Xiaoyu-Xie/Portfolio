/*
 *  This file is for use by students to define anything they wish.  It is used by both proxy server implementation
 */
#ifndef __SERVER_STUDENT_H__
#define __SERVER_STUDENT_H__

#include <curl/curl.h>
#include "gfserver.h"

struct writeinfo_t {
	char *url;
	gfcontext_t *ctx;
};
/**********************************************************************
write_data

Function called by the Libcurl library when data is received. Once the
data is received it is sent to the client using the GETFILE protocol
**********************************************************************/
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp){
/*
* Declare and initialize variables
*/
	size_t bytes_transferred;			//Keeps track of how many bytes have been transferred to the client
	ssize_t write_len;					//Records how many bytes are sent every time gfs_send is called
	struct writeinfo_t* write_info;		//Holds the gfcontext and url of the transfer
	
	printf("WRITE_DATA: Data Size: %d\n", (int) nmemb);
	
	write_info = (struct writeinfo_t*) userp;

/*
* Send data
*/
	//Sending the file contents chunk by chunk until all of data in buffer has been sent
	bytes_transferred = 0;
	while(bytes_transferred < nmemb){
		write_len = gfs_send(write_info->ctx, buffer + bytes_transferred, nmemb - bytes_transferred);
		bytes_transferred += write_len;
	}

	return bytes_transferred;
}

/**********************************************************************
getFileSize

Function uses libcurl to get the header information of the file
transfer and return it. If there are errors or the file cannot be found,
error values are returned.
**********************************************************************/
int getFileSize(CURL* curl_handle, char* url){
/*
* Declare and initialize variables
*/
	double content_length;		//Holds the length of the file to be transferred
	long response_code;			//Holds the response value from curl_easy_perform when retrieving the header
	
	//Set options for curl_easy_perform
	curl_easy_setopt(curl_handle, CURLOPT_URL, url);
	curl_easy_setopt(curl_handle, CURLOPT_HEADER, 1);
	curl_easy_setopt(curl_handle, CURLOPT_NOBODY, 1);
	
/*
* Get header info and return
*/
	//Request the header
	response_code = curl_easy_perform(curl_handle); 
	
	//Get the file size from the header information
	curl_easy_getinfo(curl_handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &content_length);
	
	
	printf("\nHANDLE: Content Length: %d\n", (int) content_length);
	printf("HANDLE: Response Code: %d\n", (int) response_code);

	
	//If response code is not 0, there was a transfer error
	if((int) response_code != 0)
	{
		return -1;
	}
	//If the content_length is -1 and response code 0, the file could not be found
	else if((int) content_length == -1)
	{
		printf("HANDLE: FILE NOT FOUND\n");
		return -2;
	}
	
	return (int) content_length;
}

/**********************************************************************
transferFile

Function uses libcurl to get the target file from the source.
**********************************************************************/
void transferFile(CURL* curl_handle, struct writeinfo_t write_info)
{
	//Set transfer options for curl_easy_perform
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data); 
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &write_info);
	curl_easy_setopt(curl_handle, CURLOPT_HEADER, 0);
	curl_easy_setopt(curl_handle, CURLOPT_NOBODY, 0);
	
	//Get the file
	curl_easy_perform(curl_handle); 
}
 
#endif // __SERVER_STUDENT_H__