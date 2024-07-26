#pragma once

#define LL long long int

#define LOOP_NUM 5
 
typedef  int borrow_c;  
#define COUNTER_SIZE  8
#define l_sign 2

#define FILTER_SIZE 32

#define MAX_INT_CNT 2147483647
#define MIN_INT_CNT -1<<31
const int MAX_CNT_CO_P = (1 << (COUNTER_SIZE - l_sign)) - 1;
const int MAX_BORROW_CO = (1 << (COUNTER_SIZE - l_sign) * 2) - 1;
const int SIGN_ONES = ((1 << l_sign) - 1) << (COUNTER_SIZE - l_sign);

const borrow_c sign_high_bit = 1<<(COUNTER_SIZE - 1);  
const borrow_c co_high_bit = 1<<(COUNTER_SIZE - l_sign - 1);
const borrow_c second_co_high_bit = 1<<(COUNTER_SIZE - l_sign - 2);


