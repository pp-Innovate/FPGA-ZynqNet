#include "Controller.h"

inline data_t out_shifter(adder_tree_t raw, ap_int<7> scale_bits)
{
	if(scale_bits > 0)
		return raw << scale_bits;
	else
		return (raw + (1 << -(scale_bits + 1))) >> -scale_bits;
}

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
	)
{
#pragma HLS ARRAY_PARTITION variable=w_in complete dim=0
#pragma HLS ARRAY_PARTITION variable=b_in complete dim=1
#pragma HLS RESOURCE variable=dst core=RAM_1P_BRAM
#pragma HLS ARRAY_PARTITION variable=dst complete dim=1
#pragma HLS INTERFACE ap_memory port=dst
#pragma HLS RESOURCE variable=src core=RAM_1P_BRAM
#pragma HLS ARRAY_PARTITION variable=src complete dim=1
#pragma HLS INTERFACE ap_memory port=src
	static data_t linebuf1[MACC_NUM][MAX_FRAME_WIDTH], linebuf2[MACC_NUM][MAX_FRAME_WIDTH];
#pragma HLS ARRAY_PARTITION variable=linebuf2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=linebuf1 complete dim=1
#pragma HLS RESOURCE variable=linebuf2 core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=linebuf1 core=RAM_S2P_BRAM
	data_t window[MACC_NUM][9];
#pragma HLS ARRAY_PARTITION variable=window complete dim=0
	data_t temp[MACC_NUM][3];
#pragma HLS ARRAY_PARTITION variable=temp complete dim=0
	ap_uint<2> irem = 0, jrem = 0;
	adder_tree_t pe_out[PE_NUM];
#pragma HLS ARRAY_PARTITION variable=pe_out complete dim=1
	data_t pe_out_scaled[PE_NUM];
#pragma HLS ARRAY_PARTITION variable=pe_out_scaled complete dim=1
	static adder_tree_intermediate_t i_buf[PE_NUM][MAX_INTERMEDIATE_LENGTH];
#pragma HLS RESOURCE variable=i_buf core=RAM_S2P_BRAM
#pragma HLS ARRAY_PARTITION variable=i_buf complete dim=1

	ap_uint<17> src_buf_addr = 0;
	ap_uint<15> dst_buf_addr = 0;

	ap_int<7> out_scale = fl_out - fl_in - fl_params;

EXPRI_LOOP_HEIGHT:
	for(int i = 0; i < height; i++, irem++)
	{
#pragma HLS LOOP_TRIPCOUNT max=130
EXPRI_LOOP_WIDTH:
		for(int j = 0; j < width; j++, jrem++)
		{
#pragma HLS DEPENDENCE variable=i_buf inter false
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=130
			bool border = pad && (i == 0 || i == height - 1 || j == 0 || j == width - 1);

EXPRI_LOOP_WINDOW:
			for(int k = 0; k < MACC_NUM; k++)
			{
#pragma HLS UNROLL
				window_generator(border ? 0 : src[k][src_buf_addr], window[k], j, linebuf1[k], linebuf2[k], temp[k]);
			}
			if(!border)
				src_buf_addr++;

			if(jrem == stride)
				jrem = 0;
			if(irem == stride)
				irem = 0;

			bool valid = (j > 1) && (i > 1) && !jrem && !irem;
			if(valid)
			{
EXPRI_LOOP_PROC:
				for(int k = 0; k < PE_NUM; k++)
				{
#pragma HLS UNROLL
					pe_out[k] = processing_element_3x3(
							window,
							w_in[k],
							b_in[k],
							first ? (adder_tree_intermediate_t)0 : i_buf[k][dst_buf_addr],
							last ? 1 : 0,
							first ? 0 : 1,
							last ? 1 : 0,
							fl_in
						);

					pe_out_scaled[k] = out_shifter(pe_out[k], out_scale);

					if(last)
						dst[k][dst_buf_addr] = pe_out_scaled[k];
					else
						i_buf[k][dst_buf_addr] = pe_out[k];
				}

				dst_buf_addr++;
			}
		}
	}
}
