#ifndef __PROCESSING_ELEMENT_H
#define __PROCESSING_ELEMENT_H

#include "hls_stream.h"
#include "Types.h"

#ifndef __SYNTHESIS__

#include <iostream>
#include <fstream>

using namespace std;

#endif /* __SYNTHESIS__ */

#define MACC_NUM 32

macc_t macc_3x3(data_t d_in[9], weight_t w_in[9]);
adder_tree_t adder_tree(macc_t d_in[MACC_NUM], macc_t bias_in, adder_tree_intermediate_t intermediate_in);
adder_tree_t processing_element_3x3(
		data_t d_in[MACC_NUM][9],
		weight_t w_in[MACC_NUM][9],
		weight_t b_in,
		adder_tree_intermediate_t i_in,
		bool add_bias,
		bool add_intermediate,
		bool activation,
		ap_int<5> fl_in
	);
adder_tree_t processing_element_1x1(
		data_t d_in[MACC_NUM],
		weight_t w_in[MACC_NUM],
		weight_t b_in,
		adder_tree_intermediate_t i_in,
		bool add_bias,
		bool add_intermediate,
		bool activation,
		ap_int<5> fl_in
	);

#endif /* __PROCESSING_ELEMENT_H */
