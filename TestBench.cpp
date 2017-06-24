#include <iostream>
#include <iomanip>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "hls_stream.h"

#include "Types.h"
#include "ProcessingElement.h"
#include "Buffers.h"
#include "Controller.h"

using namespace std;

int macc_3x3_test(void);
int adder_tree_test(void);
int window_generator_test(void);
int processing_element_3x3_test(void);
int processing_element_1x1_test(void);
int controller_3x3_test(void);
int load_weight_bias(long start, long length, weight_t *out);
int load_data(string filename, int channel_num, int width, int height, int fl, data_t *out);

int main(void)
{
	int result = controller_3x3_test();

	if(!result)
	{
		cout<<"*************************************"<<endl;
		cout<<"             Test Pass               "<<endl;
		cout<<"*************************************"<<endl;
	}

	return result;
}

int macc_3x3_test(void)
{
	data_t d_in[9] = {
			23, 23, 31,
			7, 11, -16,
			3, -29, -85
	};

	weight_t w_in[9] = {
			38, 8, -50,
			-5, 2, 3,
			21, 13, -5
	};

	macc_t macc_result_gold = -442;

	macc_t  macc_result_dut;

	macc_result_dut = macc_3x3(d_in, w_in);

	return macc_result_dut - macc_result_gold;
}

int adder_tree_test(void)
{
	macc_t d_in[MACC_NUM] = {
			35795, 404078, 418386, 132055, -379722, -295907, -333299, -480437,
			-412152, 122099, 461017, -152615, -93713, 507876, 467223, 185225,
			512021, 279792, -171234, 170269, -268262, -214426, 188930, 29199,
			-92701, 107623, 262689, 87590, 54308, 87630, 12394, -437683
	};

	macc_t bias_in = 230235;

	adder_tree_intermediate_t intermediate_in = -156192601;

	adder_tree_t adder_tree_result_gold = -154768318;

	adder_tree_t adder_tree_result_dut = adder_tree(d_in, bias_in, intermediate_in);

	return adder_tree_result_dut - adder_tree_result_gold;
}

int window_generator_test(void)
{
	data_t d_in[16] = {
			 -40, 15, 59, 70,
			 4, 49, -36, 59,
			 14, -19, -12, -18,
			 -88, 85, -29, 49
	};

	data_t windows_gold[4][9] = {
			{-40, 15, 59, 4, 49, -36, 14, -19, -12},
			{15, 59, 70, 49, -36, 59, -19, -12, -18},
			{4, 49, -36, 14, -19, -12, -88, 85, -29},
			{49, -36, 59, -19, -12, -18, 85, -29, 49}
	};

	data_t win_out[9];
	data_t linebuf1[MAX_FRAME_WIDTH], linebuf2[MAX_FRAME_WIDTH];
	data_t temp[3];

	int cnt = 0, err = 0;

	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			window_generator(d_in[i * 4 + j], win_out, j, linebuf1, linebuf2, temp);

			if(j > 1 && i > 1)
			{
				for(int k = 0; k < 9; k++)
				{
					if(win_out[k] != windows_gold[cnt][k])
						err++;
				}

				cnt++;
			}
		}
	}

	return err;
}

int processing_element_3x3_test(void)
{
	data_t d_in[64][9] = {
			{0, 0, 0, 0, 8 , 17 , 0, 7 , 14 }, {0, 0, 0, 0, 0 , 0 , 0, 0 , 0 }, {0, 0, 0, 0, 2 , 1 , 0, 1 , 0 }, {0, 0, 0, 0, 0 , 0 , 0, 0 , 0 },
			{0, 0, 0, 0, 0 , 0 , 0, 0 , 0 }, {0, 0, 0, 0, 0 , 0 , 0, 0 , 1 }, {0, 0, 0, 0, 4 , 0 , 0, 0 , 0 }, {0, 0, 0, 0, 0 , 0 , 0, 0 , 0 },
			{0, 0, 0, 0, 0 , 0 , 0, 0 , 0 }, {0, 0, 0, 0, 0 , 0 , 0, 0 , 0 }, {0, 0, 0, 0, 0 , 0 , 0, 0 , 0 }, {0, 0, 0, 0, 0 , 0 , 0, 0 , 0 },
			{0, 0, 0, 0, 3 , 6 , 0, 5 , 7 }, {0, 0, 0, 0, 0 , 0 , 0, 0 , 0 }, {0, 0, 0, 0, 0 , 4 , 0, 13 , 10 }, {0, 0, 0, 0, 0 , 0 , 0, 0 , 0 },
			{0, 0, 0, 0, 0 , 1 , 0, 0 , 1 }, {0, 0, 0, 0, 0 , 0 , 0, 0 , 0 }, {0, 0, 0, 0, 0 , 0 , 0, 0 , 0 }, {0, 0, 0, 0, 5 , 10 , 0, 5 , 9 },
			{0, 0, 0, 0, 0 , 6 , 0, 3 , 8 }, {0, 0, 0, 0, 0 , 2 , 0, 1 , 2 }, {0, 0, 0, 0, 0 , 0 , 0, 0 , 0 }, {0, 0, 0, 0, 10 , 3 , 0, 0 , 2 },
			{0, 0, 0, 0, 3 , 6 , 0, 2 , 4 }, {0, 0, 0, 0, 0 , 0 , 0, 0 , 0 }, {0, 0, 0, 0, 0 , 1 , 0, 2 , 2 }, {0, 0, 0, 0, 0 , 0 , 0, 0 , 0 },
			{0, 0, 0, 0, 2 , 4 , 0, 4 , 3 }, {0, 0, 0, 0, 0 , 0 , 0, 0 , 0 }, {0, 0, 0, 0, 0 , 1 , 0, 4 , 0 }, {0, 0, 0, 0, 0 , 0 , 0, 0 , 0 },
			{0, 0, 0, 0, 1 , 0 , 0, 0 , 0 }, {0, 0, 0, 0, 1 , 0 , 0, 0 , 0 }, {0, 0, 0, 0, 0 , 0 , 0, 0 , 0 }, {0, 0, 0, 0, 0 , 0 , 0, 0 , 0 },
			{0, 0, 0, 0, 0 , 0 , 0, 0 , 0 }, {0, 0, 0, 0, 0 , 0 , 0, 0 , 0 }, {0, 0, 0, 0, 0 , 0 , 0, 0 , 0 }, {0, 0, 0, 0, 2 , 6 , 0, 3 , 6 },
			{0, 0, 0, 0, 2 , 0 , 0, 0 , 0 }, {0, 0, 0, 0, 0 , 2 , 0, 8 , 8 }, {0, 0, 0, 0, 2 , 5 , 0, 3 , 4 }, {0, 0, 0, 0, 0 , 0 , 0, 0 , 0 },
			{0, 0, 0, 0, 1 , 0 , 0, 0 , 0 }, {0, 0, 0, 0, 1 , 0 , 0, 0 , 1 }, {0, 0, 0, 0, 0 , 0 , 0, 1 , 4 }, {0, 0, 0, 0, 2 , 2 , 0, 2 , 3 },
			{0, 0, 0, 0, 0 , 1 , 0, 1 , 2 }, {0, 0, 0, 0, 10 , 0 , 0, 0 , 0 }, {0, 0, 0, 0, 3 , 5 , 0, 3 , 5 }, {0, 0, 0, 0, 0 , 0 , 0, 0 , 0 },
			{0, 0, 0, 0, 3 , 7 , 0, 3 , 6 }, {0, 0, 0, 0, 0 , 0 , 0, 0 , 3 }, {0, 0, 0, 0, 0 , 0 , 0, 0 , 0 }, {0, 0, 0, 0, 0 , 0 , 0, 0 , 0 },
			{0, 0, 0, 0, 3 , 5 , 0, 3 , 5 }, {0, 0, 0, 0, 1 , 3 , 0, 3 , 4 }, {0, 0, 0, 0, 1 , 0 , 0, 3 , 0 }, {0, 0, 0, 0, 0 , 0 , 0, 1 , 1 },
			{0, 0, 0, 0, 0 , 0 , 0, 0 , 0 }, {0, 0, 0, 0, 3 , 5 , 0, 0 , 5 }, {0, 0, 0, 0, 0 , 0 , 0, 1 , 3 }, {0, 0, 0, 0, 0 , 0 , 0, 0 , 0 }
	};

	weight_t w_in[64][9] = {
			{1, 1, -1, 2, 1, -2, 2, 0, -3}, {-1, 1, 2, -3, 0, 5, -1, -2, 4}, {0, 8, -5, -2, 13, -9, -1, 0, -3}, {0, 0, 0, 2, 0, -1, 1, 2, -1},
			{1, 1, -1, 1, 0, 0, 1, -1, 2}, {1, -3, 3, 0, -4, 3, 1, 2, 1}, {3, 7, -6, 5, 19, -18, 2, 9, -12}, {0, 0, 0, 0, -1, 1, 0, -1, 1},
			{8, 8, 0, 18, 10, -1, 7, 6, -1}, {-1, 0, 1, -2, -1, 5, -1, -1, 4}, {4, 4, -1, 11, 5, -2, 4, 3, -2}, {-2, -2, 2, -3, 0, 8, -1, 2, 5},
			{2, 1, -1, 2, 0, -2, 1, -1, -3}, {1, 0, -2, 5, 1, -3, 2, 3, -3}, {3, 2, -2, -1, -7, 8, 4, 5, 0}, {-1, -2, 0, 0, -2, 2, 0, -1, 2},
			{3, -4, 2, 2, -8, 1, 3, -6, 0}, {0, 0, 0, 0, -1, 0, 0, -1, 0}, {2, -2, 2, 1, -6, 3, 2, -3, 3}, {-1, -1, -1, -1, -2, -1, 0, -2, -2},
			{0, -3, 5, -1, -11, 9, 0, -3, 7}, {0, -2, 2, -2, -1, 5, 0, 2, 2}, {15, 19, 28, 26, 22, 49, 14, 17, 28}, {5, 28, 12, 11, 73, 21, 13, 56, 19},
			{0, 2, -2, 2, 6, -5, 0, 3, -4}, {-1, 8, 0, -6, 5, 4, -3, -3, 6}, {0, 5, 2, -1, 3, 4, -1, -1, 5}, {9, 7, -3, 28, 12, 3, 26, 15, 4},
			{1, 6, -4, -1, 3, -3, -1, -5, 0}, {0, -2, 0, 0, -3, 1, 0, -1, 1}, {-2, 3, 10, -8, 25, 37, -2, 21, 28}, {0, -2, 2, -1, 1, 2, 0, 3, 0},
			{-3, 3, -1, -7, 17, 1, -3, 7, 2}, {-1, -4, 2, 0, 5, -4, 0, 9, -5}, {5, 2, -6, 8, 0, -8, 3, -4, -4}, {1, 0, 0, 4, 1, -2, 2, 3, -1},
			{-2, -3, 3, -4, 5, 6, -1, 7, 2}, {0, -3, 2, -1, -3, 4, 0, 2, 1}, {3, -2, 0, 4, -5, 0, 4, -4, 1}, {0, -1, 1, 0, -3, 2, -1, -2, 0},
			{2, 10, 2, 15, 23, -1, 5, 17, 2}, {0, 10, -6, 0, -2, 4, 3, -8, 5}, {3, 2, -3, 4, 1, -5, 1, -4, -2}, {1, 4, -4, 3, 13, -11, 0, 5, -7},
			{-1, 1, -5, 5, 7, -10, 1, 4, -6}, {-1, -2, 3, 2, 3, 1, 4, 6, -3}, {0, -5, 7, -1, -16, 14, 0, -5, 11}, {0, 17, 14, -5, 34, 27, 4, 14, 13},
			{-1, -1, 2, -3, -2, 6, -1, 1, 4}, {-1, -4, 5, 3, 17, -4, -1, 6, 1}, {2, 2, -2, 7, 4, -6, 3, 3, -5}, {1, -1, 0, 2, -1, -1, 1, 1, -1},
			{-2, -3, 4, -7, 3, 7, -2, 5, 3}, {2, -12, 7, 0, -2, 3, 0, 19, -6}, {0, -1, -1, 2, -1, -1, 0, 0, 0}, {0, -1, 0, 3, 0, -2, 1, 2, -2},
			{0, 3, -2, 2, 8, -4, 0, 3, -4}, {-3, 3, 4, -6, 10, 11, -3, 4, 4}, {0, 11, -8, -1, 10, -6, -2, -12, 7}, {0, 3, 0, 0, 1, 2, 0, -1, 2},
			{-1, -5, 3, -1, -1, 3, 1, 5, 1}, {2, -2, -3, 9, -3, -9, 5, 3, -8}, {1, -2, 3, 0, -8, 7, 0, -3, 5}, {1, -1, -2, 6, 0, -6, 4, 3, -5}
	};

	weight_t b_in = 2;

	adder_tree_t result_gold = 6;

	adder_tree_intermediate_t i_in = processing_element_3x3((data_t (*)[9])d_in[0], (weight_t (*)[9])w_in[0], 0, 0, 0, 0, 0, -3);
	adder_tree_t result_dut = processing_element_3x3((data_t (*)[9])d_in[32], (weight_t (*)[9])w_in[32], b_in, i_in, 1, 1, 1, -3);

	return round(result_dut/pow(2, 6-3)/pow(2, 5)) - result_gold;
}

int processing_element_1x1_test(void)
{
	data_t d_in[32] = {
			0, 6, 15, 6, 3, 0, 0, 0, 0, 8, 2, 0, 0, 4, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	weight_t w_in[32] = {
			0, 27, -27, 52, 27, 0, 0, 0, 0, 33, -17, 0, 2, -26, 41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	weight_t b_in = 1;

	adder_tree_t result_gold = 10;

	adder_tree_t result_dut = processing_element_1x1(d_in, w_in, b_in, 0, 1, 0, 1, -5);

	return round(result_dut/pow(2, 6-5)/pow(2, 5)) - result_gold;
}

int controller_3x3_test(void)
{
	weight_t *fire2_squeeze3x3_w = new weight_t[9216];
	weight_t *fire2_squeeze3x3_b = new weight_t[16];

	load_weight_bias(1792, 9216, fire2_squeeze3x3_w);
	load_weight_bias(11008, 16, fire2_squeeze3x3_b);

	data_t *conv1_d = new data_t[64 * 128 * 128];
	data_t *fire2_squeeze3x3_d = new data_t[16 * 64 * 64];

	load_data("data/conv1.dat", 64, 128, 128, -3, conv1_d);
	load_data("data/fire2_squeeze3x3.dat", 16, 64, 64, -5, fire2_squeeze3x3_d);

	data_t *sources = new data_t[MACC_NUM * 256 * 256];
	data_t destination[PE_NUM][128 * 128];
	weight_t weights[PE_NUM][MACC_NUM][9];

	int err = 0;

	for(int i = 0; i < MACC_NUM; i++)
		for(int j = 0; j < 128 * 128; j++)
			sources[i * 256 * 256 + j] = conv1_d[i * 128 * 128 + j];

	for(int i = 0; i < PE_NUM; i++)
		for(int j = 0; j < MACC_NUM; j++)
			for(int k = 0; k < 9; k++)
				weights[i][j][k] = fire2_squeeze3x3_w[i * 64 * 9 + j * 9 + k];

	controller_3x3(
			(data_t (*)[INPUT_BUF_DEPTH])sources,
			128 + 2,
			128 + 2,
			2,
			1,
			weights,
			&fire2_squeeze3x3_b[0],
			1,
			0,
			(data_t (*)[OUTPUT_BUF_DEPTH])destination,
			-3,
			-5,
			6
	);

	for(int i = 0; i < MACC_NUM; i++)
		for(int j = 0; j < 128 * 128; j++)
			sources[i * 256 * 256 + j] = conv1_d[(i + MACC_NUM) * 128 * 128 + j];

	for(int i = 0; i < PE_NUM; i++)
		for(int j = 0; j < MACC_NUM; j++)
			for(int k = 0; k < 9; k++)
				weights[i][j][k] = fire2_squeeze3x3_w[i * 64 * 9 + (j + MACC_NUM) * 9 + k];

	controller_3x3(
			(data_t (*)[INPUT_BUF_DEPTH])sources,
			128 + 2,
			128 + 2,
			2,
			1,
			weights,
			&fire2_squeeze3x3_b[0],
			0,
			1,
			(data_t (*)[OUTPUT_BUF_DEPTH])destination,
			-3,
			-5,
			6
	);

	for(int i = 0; i < PE_NUM; i++)
	{
		for(int j = 0; j < 64; j++)
		{
			for(int k = 0; k < 64; k++)
			{
				data_t result_gold = fire2_squeeze3x3_d[i * 64 * 64 + j * 64 + k];
				data_t result_dut = destination[i][j * 64 + k];

				if(result_dut - result_gold)
					err++;
			}
		}
	}

	delete[] fire2_squeeze3x3_w;
	delete[] fire2_squeeze3x3_b;
	delete[] conv1_d;
	delete[] fire2_squeeze3x3_d;
	delete[] sources;

	return err;
}

int load_weight_bias(long start, long length, weight_t *out)
{
	ifstream ifile("data/ZynqNet_Quantized_8bit.dat", ios::in | ios::binary);
	if(!ifile.is_open())
		return -1;

	ifile.seekg(start, ios::beg);
	ifile.read(out, length);

	ifile.close();

	return 0;
}

int load_data(string filename, int channel_num, int width, int height, int fl, data_t *out)
{
	ifstream ifile(filename.c_str(), ios::in | ios::binary);
	if(!ifile.is_open())
		return -1;

	int length = channel_num * height * width;
	long size = length * 4;

	char *readbuf = new char[size];

	ifile.read(readbuf, size);

	for(int i = 0; i < length; i++)
		out[i] = ((float*)readbuf)[i] * pow(2, fl);

	delete[] readbuf;

	ifile.close();

	return 0;
}
