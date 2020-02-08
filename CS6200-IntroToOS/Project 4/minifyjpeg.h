/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _MINIFYJPEG_H_RPCGEN
#define _MINIFYJPEG_H_RPCGEN

#include <rpc/rpc.h>

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif


struct minify_in {
	struct {
		u_int src_val_len;
		char *src_val_val;
	} src_val;
};
typedef struct minify_in minify_in;

struct minify_out {
	struct {
		u_int dst_val_len;
		char *dst_val_val;
	} dst_val;
};
typedef struct minify_out minify_out;

#define MINIFY_RPC 0x31230000
#define MINIFY_VERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define MINIFY_WITH_RPC 1
extern  enum clnt_stat minify_with_rpc_1(minify_in , minify_out *, CLIENT *);
extern  bool_t minify_with_rpc_1_svc(minify_in , minify_out *, struct svc_req *);
extern int minify_rpc_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define MINIFY_WITH_RPC 1
extern  enum clnt_stat minify_with_rpc_1();
extern  bool_t minify_with_rpc_1_svc();
extern int minify_rpc_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_minify_in (XDR *, minify_in*);
extern  bool_t xdr_minify_out (XDR *, minify_out*);

#else /* K&R C */
extern bool_t xdr_minify_in ();
extern bool_t xdr_minify_out ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_MINIFYJPEG_H_RPCGEN */
