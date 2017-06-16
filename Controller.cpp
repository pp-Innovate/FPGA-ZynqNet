#include "Controller.h"

inline data_t out_shifter(adder_tree_t raw, ap_int<7> scale_bits)
{
	if(scale_bits > 0)
		return raw << scale_bits;
	else
		return (raw + (1 << -(scale_bits + 1))) >> -scale_bits;
}

void controller_3x3(
		data_t src[INPUT_BUF_DEPTH][MACC_NUM],
		ap_uint<8> width,
		ap_uint<8> height,
		ap_uint<2> stride,
		bool pad,
		weight_t w_in[PE_NUM][MACC_NUM][9],
		weight_t b_in[PE_NUM],
		bool first,
		bool last,
		data_t dst[OUTPUT_BUF_DEPTH][PE_NUM],
		ap_int<5> fl_in,
		ap_int<5> fl_out,
		ap_int<5> fl_params
	)
{
	ap_uint<9> real_width, real_height;

	if(pad)
	{
		real_width = width + 2;
		real_height = height + 2;
	}
	else
	{
		real_width = width;
		real_height = height;
	}

	static data_t linebufs1[MACC_NUM][MAX_FRAME_WIDTH], linebufs2[MACC_NUM][MAX_FRAME_WIDTH];
	data_t temp[MACC_NUM][3];
	data_t windows[MACC_NUM][9];
	adder_tree_t pe_out[PE_NUM];
	data_t pe_out_scaled[PE_NUM];
	static adder_tree_intermediate_t i_buf[PE_NUM][MAX_INTERMEDIATE_LENGTH];

	ap_int<7> out_scale = fl_out - fl_in - fl_params;

	ap_uint<15> dst_buf_addr = 0;
	ap_uint<17> src_buf_addr = 0;
	ap_uint<2> irem = 0, jrem = 0;

CNTR3X3_LOOP_HEIGHT:
	for(int i = 0; i < real_height; i++, irem++)
	{
CNTR3X3_LOOP_WIDTH:
		for(int j = 0; j < real_width; j++, jrem++)
		{
			bool border = pad && (i == 0 || i == real_height - 1 || j == 0 || j == real_width - 1);

CNTR3X3_LOOP_WINDOW:
			for(int k = 0; k < MACC_NUM; k++)
				window_generator(border ? 0 : src[src_buf_addr][k], windows[k], j, linebufs1[k], linebufs2[k], temp[k]);

			if(!border)
				src_buf_addr++;

			if(jrem == stride)
				jrem = 0;
			if(irem == stride)
				irem = 0;

			bool valid = (j > 1) && (i > 1) && !jrem && !irem;

CNTR3X3_LOOP_PROC:
			for(int k = 0; k < PE_NUM; k++)
			{
				pe_out[k] = processing_element_3x3(
						windows,
						w_in[k],
						b_in[k],
						first ? (adder_tree_intermediate_t)0 : i_buf[k][dst_buf_addr],
						last ? 1 : 0,
						first ? 0 : 1,
						last ? 1 : 0,
						fl_in
				);

				pe_out_scaled[k] = out_shifter(pe_out[k], out_scale);
			}

			if(valid)
			{
CNTR3X3_LOOP_OUTPUT:
				for(int k = 0; k < PE_NUM; k++)
				{
					if(last)
						dst[dst_buf_addr][k] = pe_out_scaled[k];
					else
						i_buf[k][dst_buf_addr] = pe_out[k];
				}

				dst_buf_addr++;
			}
		}
	}
}
