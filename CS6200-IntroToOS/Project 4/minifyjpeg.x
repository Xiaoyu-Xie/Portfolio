/*
 * Define your service specification in this file and run rpcgen -MN minifyjpeg.x
 */
struct minify_in {
	opaque src_val<>;
};

struct minify_out {
	opaque dst_val<>;
};

program MINIFY_RPC {
	version MINIFY_VERS {
		minify_out MINIFY_WITH_RPC(minify_in) = 1;
	} = 1;
} = 0x31230000;