#ifndef __TYPES_H
#define __TYPES_H

#include "ap_int.h"
#include "ap_fixed.h"

typedef char weight_t;
typedef char data_t;
typedef short mul_t;
typedef ap_int<20> macc_t;

/* MACC_NUM == 32, PE_NUM == 2 */
typedef ap_int<30> adder_tree_intermediate_t;
typedef ap_int<31> adder_tree_t;

#endif /* __TYPES_H */
