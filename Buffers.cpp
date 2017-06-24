#include "Buffers.h"

void window_generator(
		data_t d_in,
		data_t win_out[9],
		ap_uint<9> column,
		data_t linebuf1[MAX_FRAME_WIDTH],
		data_t linebuf2[MAX_FRAME_WIDTH],
		data_t temp[3]
	)
{
#pragma HLS DEPENDENCE variable=linebuf1 inter false
#pragma HLS RESOURCE variable=linebuf2 core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=linebuf1 core=RAM_S2P_BRAM
#pragma HLS ARRAY_PARTITION variable=temp complete dim=1
#pragma HLS ARRAY_PARTITION variable=win_out complete dim=1
#pragma HLS INLINE
	temp[0] = d_in;
	temp[1] = linebuf1[column];
	temp[2] = linebuf2[column];

	win_out[6] = win_out[7];
	win_out[3] = win_out[4];
	win_out[0] = win_out[1];

	win_out[7] = win_out[8];
	win_out[4] = win_out[5];
	win_out[1] = win_out[2];

	win_out[8] = temp[0];
	win_out[5] = temp[1];
	win_out[2] = temp[2];

	linebuf1[column] = temp[0];
	linebuf2[column] = temp[1];
}
