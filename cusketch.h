#pragma once
#include "borrow.h"
#include <algorithm>
#include <cstring>
#include <string.h>
#include "bobhash.h"
#include "params.h"
#include "sketch.h"
#include <iostream>


using namespace std;

class CUSketch:public class_sketches
{
private:
    BOBHash * bobhash[20];
    int index[20];
    borrow_c* borrow_counter[20];
    int* int_counter[20];
    int w, d;
   
    int counter_index_size;
    uint64_t hash_value;

public:
    CUSketch(int _w, int _d)
    {
        counter_index_size = 20;
        w = _w;
        d = _d;

        for(int i = 0; i < d; i++)
        {
            int_counter[i] = new int[w];
            memset(int_counter[i], 0, sizeof(int) * w);
            
            borrow_counter[i] = new borrow_c[w];
            memset(borrow_counter[i], 0, sizeof(borrow_c) * w);
            bobhash[i] = new BOBHash(i + 1000);
        }          
    }
    //insert function for int counters
	void Insert(const char *str, int c) {
		int min_value = MAX_INT_CNT;
		int temp;
		for (int i = 0; i < d; i++)
		{
			index[i] = (bobhash[i]->run(str, strlen(str))) % w;
			temp = int_counter[i][index[i]];
			min_value = temp < min_value ? temp : min_value;
		}
		if (min_value + c >= MAX_INT_CNT)
			for (int i = 0; i < d; i++)
			{
				int_counter[i][index[i]] = MAX_INT_CNT;
				return;
			}
		for (int i = 0; i < d; i++)
		{
			if (int_counter[i][index[i]] == min_value) {
				int_counter[i][index[i]] += c;
			}
			else if (int_counter[i][index[i]] < min_value + c) {
				int_counter[i][index[i]] = min_value + c;
			}
		}
	}
	//query function for counters of int data type
	int Query(const char *str)
	{
		int min_value = MAX_INT_CNT;
		int temp;
		for (int i = 0; i < d; i++)
		{
			index[i] = (bobhash[i]->run(str, strlen(str))) % w;
			temp = int_counter[i][index[i]];
			min_value = temp < min_value ? temp : min_value;
		}
		return min_value;
	}
	//insert function for borrow
	void borrow_insert(const char *str, int c) { 
		int min_value = (1 << ((COUNTER_SIZE - l_sign)*2)) - 1;
		int temp;
		int index_value1, index_value2, index_value3;
		int counter_value;
		int own_counter_value;
		for (int i = 0; i < d; i++)
		{
			index[i] = (bobhash[i]->run(str, strlen(str))) % w;
			temp = get_true_value(borrow_counter[i][index[i]], borrow_counter[i][index[i] + 1]);
			min_value = temp < min_value ? temp : min_value;
		}
		
		for (int i = 0; i < d; i++)
		{
			index[i] = (bobhash[i]->run(str, strlen(str))) % w;
			counter_value = get_true_value(borrow_counter[i][index[i]], borrow_counter[i][index[i] + 1]);
								
			if (counter_value == min_value) {
			
			int sign_bits = get_signbits(borrow_counter[i][index[i]]);
			int used_bits = COUNTER_SIZE - l_sign - sign_bits;
			int borrow_bits = get_signbits(borrow_counter[i][index[i] + 1]);
			LL value = get_val(borrow_counter[i][index[i]]);
			if((borrow_bits != 0 ) && ((borrow_counter[i][index[i]+1] & (3 << (COUNTER_SIZE - l_sign - 2))) == 0)){
				big_adding(borrow_counter[i][index[i]],borrow_counter[i][index[i]+1]);				
			}
			else{
					if(value == ((1 << used_bits) - 1)){										
						bool m = add_next(borrow_counter[i][index[i] + 1], borrow_counter[i][index[i] + 2]);
						if(m == true){//borrow once														
							after_borrow(borrow_counter[i][index[i]]);																						
						}
						else if(m == false){//again borrow									
							bool n = again_borrow(borrow_counter[i][index[i]+2], borrow_counter[i][index[i]+1], borrow_counter[i][index[i]+3]);							
							if(n == true){																								
								after_again_borrow(borrow_counter[i][index[i]+1]);
								after_borrow(borrow_counter[i][index[i]]);								
							}
							else if(n == false){
								if(borrow_bits != 0){
									before_big_adding(borrow_counter[i][index[i]], borrow_counter[i][index[i]+1]);
								}								
							}
						}	
					}
					else {
						add_one(borrow_counter[i][index[i]]);
					}
			}
			}
		}	
	}
	
	//query function for borrow
	int borrow_query(const char *str) {
		int min_value = (1 << ((COUNTER_SIZE - l_sign)*2)) - 1;
		int temp;
		for (int i = 0; i < d; i++)
		{
			index[i] = (bobhash[i]->run(str, strlen(str))) % w;
			temp = get_true_value(borrow_counter[i][index[i]], borrow_counter[i][index[i] + 1]);	
			min_value = temp < min_value ? temp : min_value;
		}
		return min_value;
		
	}
	
    ~CUSketch()
    {
        for(int i = 0; i < d; i++)
        {
            delete[] int_counter[i];
            delete[] borrow_counter[i];
            delete bobhash[i];
        }
    }
};
 // CUSKETCH_H
