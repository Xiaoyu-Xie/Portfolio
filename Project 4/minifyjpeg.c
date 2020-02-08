#include "magickminify.h"
#include "minifyjpeg.h"

/* Implement the server-side functions here */
bool_t minify_with_rpc_1_svc(minify_in  input, minify_out *output, struct svc_req *rqstp) {
	void *new_img;
	
	new_img = (char *) magickminify((void *) input.src_val.src_val_val, (ssize_t) input.src_val.src_val_len, (ssize_t *) &output->dst_val.dst_val_len);
	//memcpy(output->dst_val.dst_val_val, new_img, output->dst_val.dst_val_len);
	output->dst_val.dst_val_val = new_img;
	
	printf("New Length: %d\n", (int) output->dst_val.dst_val_len);
	
	return 1;
}

/*enum clnt_stat minify_with_rpc_1(minify_in input, minify_out *output, CLIENT *cl){
	
	
	return RPC_SUCCESS;
}*/

int minify_rpc_1_freeresult(SVCXPRT *transp, xdrproc_t _xdr_result, caddr_t result){
	
	xdr_free(_xdr_result, result);
	
	return 1;
}