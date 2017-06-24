#include "ProcessingElement.h"

macc_t macc_3x3(data_t d_in[9], weight_t w_in[9])
{
#pragma HLS INLINE
#pragma HLS PIPELINE
#pragma HLS ARRAY_PARTITION variable=w_in complete dim=1
#pragma HLS ARRAY_PARTITION variable=d_in complete dim=1
	mul_t mul_result[9];
#pragma HLS ARRAY_PARTITION variable=mul_result complete dim=1
	ap_int<17> sum3[4];
#pragma HLS ARRAY_PARTITION variable=sum3 complete dim=1
	ap_int<18> sum2[2];
#pragma HLS ARRAY_PARTITION variable=sum2 complete dim=1
	ap_int<19> sum1;

MACC_MUL_LOOP:
	for(int i = 0; i < 9; i++)
	{
#pragma HLS LATENCY max=0
#pragma HLS UNROLL
		mul_result[i] = d_in[i] * w_in[i];
	}

	sum3[0] = mul_result[0] + mul_result[1];
	sum3[1] = mul_result[2] + mul_result[3];
	sum3[2] = mul_result[4] + mul_result[5];
	sum3[3] = mul_result[6] + mul_result[7];

	sum2[0] = sum3[0] + sum3[1];
	sum2[1] = sum3[2] + sum3[3];

	sum1 = sum2[0] + sum2[1];

	return sum1 + mul_result[8];
}

adder_tree_t adder_tree(macc_t d_in[MACC_NUM], macc_t bias_in, adder_tree_intermediate_t intermediate_in)
{
#pragma HLS INLINE
#pragma HLS PIPELINE
#pragma HLS ARRAY_PARTITION variable=d_in complete dim=1
#if MACC_NUM == 32

	ap_int<21> sum5[16];
#pragma HLS ARRAY_PARTITION variable=sum5 complete dim=1
	ap_int<22> sum4[8];
#pragma HLS ARRAY_PARTITION variable=sum4 complete dim=1
	ap_int<23> sum3[4];
#pragma HLS ARRAY_PARTITION variable=sum3 complete dim=1
	ap_int<24> sum2[2];
#pragma HLS ARRAY_PARTITION variable=sum2 complete dim=1
	ap_int<25> sum1;
	adder_tree_intermediate_t temp;

ADDER_TREE_LOOP1:
	for(int i = 0; i < 16; i++)
	{
#pragma HLS UNROLL
		sum5[i] = d_in[2 * i] + d_in[2 * i + 1];
	}

ADDER_TREE_LOOP2:
	for(int i = 0; i < 8; i++)
	{
#pragma HLS UNROLL
		sum4[i] = sum5[2 * i] + sum5[2 * i + 1];
	}

ADDER_TREE_LOOP3:
	for(int i = 0; i < 4; i++)
	{
#pragma HLS UNROLL
		sum3[i] = sum4[2 * i] + sum4[2 * i + 1];
	}

ADDER_TREE_LOOP4:
	for(int i = 0; i < 2; i++)
	{
#pragma HLS UNROLL
		sum2[i] = sum3[2 * i] + sum3[2 * i + 1];
	}

	sum1 = sum2[0] + sum2[1];
	temp = intermediate_in + bias_in;

	return sum1 + temp;

#endif /* MACC_NUM == 32 */
}

adder_tree_t processing_element_3x3(
		data_t d_in[MACC_NUM][9],
		weight_t w_in[MACC_NUM][9],
		weight_t b_in,
		adder_tree_intermediate_t i_in,
		bool add_bias,
		bool add_intermediate,
		bool activation,
		ap_int<5> fl_in
	)
{
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=w_in complete dim=0
#pragma HLS ARRAY_PARTITION variable=d_in dim=0
#pragma HLS PIPELINE
	macc_t bias_scaled = (fl_in > 0) ? (((macc_t)b_in) << fl_in) : (((macc_t)b_in) >> -fl_in);
	macc_t bias = add_bias ? bias_scaled : (macc_t)0;
	adder_tree_intermediate_t intermediate = add_intermediate ? i_in : (adder_tree_intermediate_t)0;

	macc_t macc_results[MACC_NUM];
#pragma HLS ARRAY_PARTITION variable=macc_results complete dim=1

PE_MACC_LOOP:
	for(int i = 0; i < MACC_NUM; i++)
	{
#pragma HLS UNROLL
		macc_results[i] = macc_3x3(d_in[i], w_in[i]);
	}

	adder_tree_t temp = adder_tree(macc_results, bias, intermediate);

	return (activation && (temp < 0)) ? (adder_tree_t)0 : temp;
}

adder_tree_t processing_element_1x1(
		data_t d_in[MACC_NUM],
		weight_t w_in[MACC_NUM],
		weight_t b_in,
		adder_tree_intermediate_t i_in,
		bool add_bias,
		bool add_intermediate,
		bool activation,
		ap_int<5> fl_in
	)
{
#pragma HLS ARRAY_PARTITION variable=w_in complete dim=1
#pragma HLS ARRAY_PARTITION variable=d_in complete dim=1
#pragma HLS PIPELINE
	macc_t bias_scaled = (fl_in > 0) ? (((macc_t)b_in) << fl_in) : (((macc_t)b_in) >> -fl_in);
	macc_t bias = add_bias ? bias_scaled : (macc_t)0;
	adder_tree_intermediate_t intermediate = add_intermediate ? i_in : (adder_tree_intermediate_t)0;

	macc_t mul_results[MACC_NUM];
#pragma HLS ARRAY_PARTITION variable=mul_results complete dim=1

PE_MUL_LOOP:
	for(int i = 0; i < MACC_NUM; i++)
	{
#pragma HLS LATENCY max=0
#pragma HLS UNROLL
		mul_results[i] = d_in[i] * w_in[i];
	}

	adder_tree_t temp = adder_tree(mul_results, bias, intermediate);

	return (activation && (temp < 0)) ? (adder_tree_t)0 : temp;
}
