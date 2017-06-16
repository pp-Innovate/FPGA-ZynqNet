#ifndef __BUFFERS_H
#define __BUFFERS_H

#include "hls_stream.h"
#include "Types.h"

#ifndef __SYNTHESIS__

#include <iostream>
#include <fstream>

using namespace std;

#endif /* __SYNTHESIS__ */

#define MAX_FRAME_WIDTH 258

void window_generator(
		data_t d_in,
		data_t win_out[9],
		ap_uint<9> column,
		data_t linebuf1[MAX_FRAME_WIDTH],
		data_t linebuf2[MAX_FRAME_WIDTH],
		data_t temp[3]
	);

#endif /* __BUFFERS_H */
