
#include<math.h>
#include<stdint.h>

typedef struct edgeCore {
	const uint8_t PipeSZ = 6;
	uint16_t pipe[PipeSZ] = {0};
	uint8_t edges = 0;
	float threshold = 0.3;
	uint16_t baseline = 2100; // gate width 2100mm
	uint8_t i = 0;
}edge_ctx;

edge_ctx VL53edge_ctx;

void VL53edge_init(edge_ctx the_ctx){
	memset(the_ctx.pipe,0,6);
	the_ctx.edges = 0;
	the_ctx.threshold = 0.3;
	the_ctx.baseline = 2100;
}

void VL53edge_push(edge_ctx the_ctx, uint16_t readin){
	for(the_ctx.i = 0; the_ctx.i<the_ctx.PipeSZ-1; the_ctx.i++){
		the_ctx.pipe[the_ctx.i] = the_ctxpipe[the_ctx.i+1];
	}
	the_ctx.pipe[the_ctx.PipeSZ-1] = readin;
}