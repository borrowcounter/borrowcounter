#pragma once 
#include <algorithm>
#include <cstring>
#include "sketch.h"
#include <string.h>
#include "bobhash.h"
#include "borrow.h"
#include <iostream>
#include "params.h"


using namespace std;
class CMSketch :public class_sketches {
private:
	BOBHash* bobhash[20];
	int index[20];
	borrow_c* counter[20];
	int* int_counter[20];
	int w, d; 
	int cc = 0;
	
	int counter_index_size;
	uint64_t hash_value;
public:
	CMSketch(int _w, int _d) {
		counter_index_size = 20;
		w = _w;
		d = _d;

		for (int i = 0; i < d; i++)
		{
			counter[i] = new borrow_c[w];
			memset(counter[i], 0, sizeof(borrow_c) * w);
			
			bobhash[i] = new BOBHash(i + 1000);

			int_counter[i] = new int[w];
			memset(int_counter[i], 0, sizeof(int) * w);
		}
	}
	//insert function for int counters
	void Insert(const char* str, int c) {
		for (int i = 0; i < d; i++) {
			index[i] = (bobhash[i]->run(str, strlen(str))) % w;
			if (int_counter[i][index[i]] + c <= MAX_INT_CNT) {
				int_counter[i][index[i]] += c;
			}
		}
	}
	//query function for int counters
	int Query(const char* str) {
		int min_value = MAX_INT_CNT;
		int temp;

		for (int i = 0; i < d; i++)
		{
			index[i] = (bobhash[i]->run(str, strlen(str))) % w;
			temp = int_counter[i][index[i]];
			//temp = short_counter[i][index[i]];
			min_value = temp < min_value ? temp : min_value;
		}
		return min_value;
	}
	 
	void borrow_insert(const char *str, int c)
	{
		for (int i = 0; i < d; i++)
		{
			index[i] = (bobhash[i]->run(str, strlen(str))) % w;
			int sign_bits = get_signbits(counter[i][index[i]]);
			int used_bits = COUNTER_SIZE - l_sign - sign_bits;
			int borrow_bits = get_signbits(counter[i][index[i] + 1]);
			LL value = get_val(counter[i][index[i]]);			
			if((borrow_bits != 0 ) && ((counter[i][index[i]+1] & (3 << (COUNTER_SIZE - l_sign - 2))) == 0)){
				big_adding(counter[i][index[i]],counter[i][index[i]+1]);				
			}
			else{
					if(value == ((1 << used_bits) - 1)){										
						bool m = add_next(counter[i][index[i] + 1], counter[i][index[i] + 2]);
						if(m == true){//borrow once														
							after_borrow(counter[i][index[i]]);																						
						}
						else if(m == false){//again borrow									
							bool n = again_borrow(counter[i][index[i]+2], counter[i][index[i]+1], counter[i][index[i]+3]);							
							if(n == true){																								
								after_again_borrow(counter[i][index[i]+1]);
								after_borrow(counter[i][index[i]]);								
							}
							else if(n == false){
								if(borrow_bits != 0){
									before_big_adding(counter[i][index[i]], counter[i][index[i]+1]);
								}								
							}
						}	
					}
					else {
						add_one(counter[i][index[i]]);
					}
			}	
		}
		
	}
	
	int borrow_query(const char* str)
	{
		LL min_value = (1 << ((COUNTER_SIZE - l_sign)*2)) - 1;
		LL temp;
	
		for (int i = 0; i < d; i++)
		{
			index[i] = (bobhash[i]->run(str, strlen(str))) % w;
			temp = get_true_value(counter[i][index[i]], counter[i][index[i] + 1]);
			min_value = min(min_value, temp);
		}
		return min_value;

	}
	
	~CMSketch() {
		for (int i = 0; i < d; ++i)
		{
			delete[] counter[i];
			delete[] int_counter[i];
			//delete[] short_counter[i];
			delete bobhash[i];
		}
			}
};

// CMSKETCH_H
