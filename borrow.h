#pragma once

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <iostream>
#include "params.h"

using namespace std;


void bprint(borrow_c x) { //print binary value of x
	for (int i =COUNTER_SIZE-1; i >= 0; --i) {
		printf("%hhd", (x & (1<<i)) >> i);
	}
	printf("\n");
}

//get value of sign bits
int get_signbits(borrow_c x){
	int sign_bits = 0;
	borrow_c delta = sign_high_bit;
	for (int i = 1; i <= l_sign; ++i) {
		if (x & delta) {
		    sign_bits += 1<<(l_sign - i);
		}
		delta = delta >> 1;
	}
	return sign_bits * 2;
}

//get value of own used bits
LL get_val(borrow_c x) {  
	int sign_bits = 0;
	int used_bits = 0;
	LL value = 0;
	sign_bits = get_signbits(x);
	used_bits = COUNTER_SIZE - l_sign - sign_bits;
        value = (((1 << used_bits) - 1) & x);
	return value;
}

LL get_borrow_val(borrow_c x){
	borrow_c delta = co_high_bit;
	int sign_bits = 0;
	LL borrow_value = 0;
	sign_bits = get_signbits(x);
	for (int i = 1; i <= sign_bits; ++i) {
		if (x & delta) {
		    borrow_value += 1<<(sign_bits - i);
		}
		delta = delta >> 1;
	}
	return borrow_value;
}

LL big_get_val(borrow_c x, borrow_c y){
	unsigned short pos = 0;
	LL answer = 0;	
	int sign_bits = 0;
	int used_bits = 0;
	sign_bits = get_signbits(y);
	used_bits = COUNTER_SIZE - l_sign - get_signbits(x);
	LL x_true = get_val(x) + (get_borrow_val(y) << used_bits);
	pos = __builtin_clz(x_true) - (32 - (sign_bits + used_bits));
	borrow_c ones = (1 << (sign_bits + used_bits - pos - 1)) - 1;	
	answer += ((ones & x_true) << (2 * pos)) + ((1 << (2 * pos)) - 1);
	for (int i = 1; i < pos ; ++i) {
		answer += ((1 << (sign_bits + used_bits + i - 1)) - 1);
	}
	return answer;
}

LL get_true_value(borrow_c x, borrow_c y){
	LL true_value = 0;
	int sign_y = 0;
	int borrow_y = 0;
	sign_y = get_signbits(y);
	borrow_y = get_borrow_val(y);
	if(sign_y == 0){
		true_value = get_val(x);
	}
	else if((sign_y != 1 && borrow_y >= (1 << (sign_y - 2))) ){
		true_value = ( get_borrow_val(y) << (COUNTER_SIZE - l_sign- get_signbits(x)) )+ get_val(x);
	}
	else{
		true_value = big_get_val(x, y);
	}
	return true_value;
}

bool add_one(borrow_c& x) {
	int sign_bits = 0;
	int used_bits = 0;
	LL value = 0;
	sign_bits = get_signbits(x);
	used_bits = COUNTER_SIZE - l_sign - sign_bits;
	value = get_val(x);
	if(value != ((1 << used_bits) - 1)){
		++x;
		return true;
	}
	return false;
}

bool minus_one(borrow_c& x) {
	LL value = 0;
	value = get_val(x);
	if(value != 0){
		--x;
		return true;
	}	
	return false;
}

bool add_next(borrow_c& x, borrow_c& y){//second counter, third counter
	int sign_bits = 0;
	int used_bits = 0;
	LL value = 0;
	LL borrow_value = 0;
	sign_bits = get_signbits(x);
	int y_sign_bits = get_signbits(y);
	used_bits = COUNTER_SIZE - l_sign - sign_bits;
	value = get_val(x);
	borrow_value = get_borrow_val(x);
	if(borrow_value != ((1 << sign_bits) - 1)){
		x += (1 << used_bits);
		return true;
	}
	else{
		if(used_bits == 0)
			return false;
		else if((y_sign_bits == 0)  && !(x & (1 << (used_bits - 1))) && !(x & (1 << (used_bits - 2)))){
			x += (1 << (COUNTER_SIZE - l_sign));//sign + 2
			x -= (borrow_value << used_bits);
			x += second_co_high_bit;
			return true;
		}	
	}
	return false;
}
	
bool minus_next(borrow_c& x){//second counter
	int sign_bits = 0;
	int used_bits = 0;
	sign_bits = get_signbits(x);
	used_bits = COUNTER_SIZE - l_sign - sign_bits;
	x -= (1 << used_bits);
	return true;
}

void after_minus(borrow_c& x){
	int sign_bits = 0;
	int used_bits = 0;
	sign_bits = get_signbits(x);
	used_bits = COUNTER_SIZE - l_sign - sign_bits;
	x &= ((1 << used_bits ) - 1);//own used bits set 1
	return;
}

void after_borrow(borrow_c& x){
	int sign_bits = 0;
	int used_bits = 0;
	sign_bits = get_signbits(x);
	used_bits = COUNTER_SIZE - l_sign - sign_bits;
	x &= (((1 << (sign_bits + l_sign)) - 1) << used_bits);//own used bits set 0
	return;
}

bool again_borrow(borrow_c& x ,borrow_c& y, borrow_c& z){//third counter,second counter,forth counter
	int sign_bits = 0;
	int used_bits = 0;
	LL x_value = 0;
	LL y_value = 0;
	LL borrow_value = 0;
	sign_bits = get_signbits(x);
	int z_sign_bits = get_signbits(z);
	int y_used_bits = COUNTER_SIZE - l_sign - get_signbits(y);
	used_bits = COUNTER_SIZE - l_sign - sign_bits;
	x_value = get_val(x);
	y_value = get_val(y);
	borrow_value = get_borrow_val(x);
	if((y_used_bits != 0) && (used_bits != 0) &&  (z_sign_bits == 0) && !(x & (1 << (used_bits - 1))) && !(x & (1 << (used_bits - 2)))){
		x += (1 << COUNTER_SIZE - l_sign);
		x += (y_value & 3) << (used_bits - 2);
		return true;

	}		
	return false;
}

void after_again_borrow(borrow_c& x){//second counter
	int used_bits = 0;
	LL value = 0;
	used_bits = COUNTER_SIZE - l_sign - get_signbits(x);
	value = get_val(x);
	x += (1 << COUNTER_SIZE - l_sign);
	x &= SIGN_ONES;//set counter bits 0
	x += second_co_high_bit + (value & ((1 << (used_bits - 2)) - 1));
}



void before_big_adding(borrow_c& x, borrow_c& y){
	int sign_bits = 0;
	int used_bits = 0;
	sign_bits = get_signbits(y);
	used_bits = COUNTER_SIZE - l_sign - get_signbits(x);
	LL y_value = get_val(y);
	y &= SIGN_ONES;
	y += y_value;
	after_borrow(x);
	if(sign_bits > 2){
		y += (1 << (COUNTER_SIZE - l_sign - 3));
	}
	else if(used_bits > 0){
		x += 1 << (used_bits - 1);
	}
}

void big_adding(borrow_c& x, borrow_c& y) {//first,second
	unsigned short pos = 0;
	int sign_bits = 0;
	int used_bits = 0;
	sign_bits = get_signbits(y);
	used_bits = COUNTER_SIZE - l_sign - get_signbits(x);
	LL y_value = get_val(y);
	
	LL x_val = get_val(x) + (get_borrow_val(y) << used_bits);		
	pos = __builtin_clz(x_val) - (32 - (sign_bits + used_bits));
	LL x_true = get_val(x) + ((get_borrow_val(y) - (1 << (sign_bits - pos - 1)))<< used_bits);
	LL ones = (1 << (sign_bits + used_bits - pos - 1)) - 1;
	double add = (double)1 / (1 << (2 * pos));	
	if ((ones & x_true) != ones) {
		if (drand48() < add){
			if(get_val(x) != ((1 << used_bits) - 1)){
				++x;
				return;
			}
			else{
				y += (1 << (COUNTER_SIZE - l_sign - sign_bits));
				after_borrow(x);
				return;
			}			
		}
		return;	
	}
	else{
		if(sign_bits >= (pos + 2)){
			after_borrow(x);
			y &= SIGN_ONES;
			y += y_value + (1 << (COUNTER_SIZE - l_sign - pos - 2));
			return;
		}
		else if(used_bits >= (pos + 2 - sign_bits)){
			y &= SIGN_ONES;
			y += y_value;
			after_borrow(x);
			x += 1 << (used_bits - pos - 2 + sign_bits);
			return;
		}
	}
	
}
