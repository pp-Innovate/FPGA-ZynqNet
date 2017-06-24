#ifndef __CONTROLL_H
#define __CONTROLL_H

#include "hls_stream.h"
#include "Types.h"
#include "ProcessingElement.h"
#include "Buffers.h"

#ifndef __SYNTHESIS__

#include <iostream>
#include <fstream>

using namespace std;

#endif /* __SYNTHESIS__ */

#define PE_NUM 2
#define MAX_INTERMEDIATE_LENGTH 128 * 128
#define INPUT_BUF_DEPTH 256 * 256
#define OUTPUT_BUF_DEPTH 128 * 128

void controller_3x3(
		data_t src[MACC_NUM][INPUT_BUF_DEPTH],
		ap_uint<8> width,
		ap_uint<8> height,
		ap_uint<2> stride,
		bool pad,
		weight_t w_in[PE_NUM][MACC_NUM][9],
		weight_t b_in[PE_NUM],
		bool first,
		bool last,
		data_t dst[PE_NUM][OUTPUT_BUF_DEPTH],
		ap_int<5> fl_in,
		ap_int<5> fl_out,
		ap_int<5> fl_params
	);

#endif /* __CONTROLL_H */
