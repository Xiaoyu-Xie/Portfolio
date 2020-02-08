#include "minifyjpeg_clnt.c"
#include "minifyjpeg_xdr.c"
#include "minifyjpeg.h"
#include <stdio.h>
#include <stdlib.h>

CLIENT* get_minify_client(char *server){
    CLIENT *cl;
	struct timeval tv;
	
	tv.tv_sec =3;
	tv.tv_usec = 0;

    /* Your code goes here */
	cl = clnt_create(server, MINIFY_RPC, MINIFY_VERS, "tcp");
	clnt_control(cl, CLSET_TIMEOUT, (char *) &tv);
	
    return cl;
}

/*
The size of the minified file is not known before the call to the library that minimizes it,
therefore this function should allocate the right amount of memory to hold the minimized file
and return it in the last parameter to it
*/
int minify_via_rpc(CLIENT *cl, void* src_val, size_t src_len, size_t *dst_len, void **dst_val){
	minify_in input;
	minify_out output;
	enum clnt_stat result;
	
	output.dst_val.dst_val_val = (char *) malloc(sizeof(char) * src_len);
	
	//input.src_val.src_val_val = (char *) malloc(sizeof(char) * src_len);
	input.src_val.src_val_val = (char *) src_val;
	
	//memcpy(input.src_val.src_val_val, (char *) src_val, src_len);
	input.src_val.src_val_len = (int) src_len;

	/*Your code goes here */
    result = minify_with_rpc_1(input, &output, cl);
	
	if(result == RPC_TIMEDOUT)
	{
		//free(input.src_val.src_val_val);
		free(output.dst_val.dst_val_val);
		return RPC_TIMEDOUT;
	}
	
	printf("New Length: %d\n", (int) output.dst_val.dst_val_len);
	
	*dst_val = output.dst_val.dst_val_val;
	
	*dst_len = output.dst_val.dst_val_len;
	
	return 0;
}
