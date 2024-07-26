#pragma once
#include <algorithm>
#include <cstring>
#include <string.h>
#include <vector>
#include "sketch.h"
#include "bobhash.h"
#include "params.h"
#include "borrow.h"
#include <iostream>

class CSketch:public class_sketches{	
public:
    BOBHash * bobhash[20 * 2];
    int index[20];
    int *counter[20];
    borrow_c *sign_counter[20];
    
    
    int w, d;
    int base_num=1;
    
	
    int counter_index_size;
    uint64_t hash_value;
	
    CSketch(int _w, int _d){
	counter_index_size = 20;
        w = _w/base_num;
        d = _d/base_num;
		
        for(int i = 0; i < _d; i++){
            counter[i] = new int[_w];
            memset(counter[i], 0, sizeof(int) * _w);

            sign_counter[i] = new borrow_c[_w];
            memset(sign_counter[i], 0, sizeof(borrow_c) * _w);
	}
	
	
        for(int i = 0; i < _d * 2; i++)
	    bobhash[i] = new BOBHash(i + 1000);
		
    }
	
    //The insert function for int counters
    void Insert(const char * str,int c){
	int g = 0;
        for(int i = 0; i < d; i++){
	    index[i] = (bobhash[i]->run(str, strlen(str))) % w;
	    g = (bobhash[i + d]->run(str, strlen(str))) % 2;

	    if(g == 0){
                if(counter[i][index[i]]+c < MAX_INT_CNT)
                    counter[i][index[i]] += c;
                else cout<<"int overflow, counter value = "<< counter[i][index[i]]<<endl;
	    }
	    else{
                if(counter[i][index[i]]-c > MIN_INT_CNT)
                    counter[i][index[i]] -= c;
                else cout<<"int underflow, counter value = "<< counter[i][index[i]]<<endl;
	    }
	}
    }
    
    //the query function for int counters
    int Query(const char *str)
	{
		int temp;
		int res[20];
		int g;
		for (int i = 0; i < d; i++)
		{
			index[i] = (bobhash[i]->run(str, strlen(str))) % w;
			temp = counter[i][index[i]];
			g = (bobhash[i + d]->run(str, strlen(str))) % 2;
			res[i] = (g == 0 ? temp : -temp);
		}
		sort(res, res + d);
		int r;
		if (d % 2 == 0)
		{
			r = (res[d / 2] + res[d / 2 - 1]) / 2;
		}
		else
		{
			r = res[d / 2];
		}
		return r;
	}
	//the delete function for int counters
	void Delete(const char * str)
	{
		int g = 0;
		for (int i = 0; i < d; i++)
		{
			index[i] = (bobhash[i]->run(str, strlen(str))) % w;
			g = (bobhash[i + d]->run(str, strlen(str))) % 2;

			if (g == 1)
			{
				if (counter[i][index[i]] != MAX_INT_CNT)
				{
					counter[i][index[i]]++;
				}
			}
			else
			{
				if (counter[i][index[i]] != MIN_INT_CNT)
				{
					counter[i][index[i]]--;
				}
			}
		}
	}
	//the insert functions for borrow
    void borrow_insert(const char *str,int c){

        int g = 0;
        for(int i = 0; i < d; i++)
        {
            index[i] = (bobhash[i]->run(str, strlen(str))) % w;
            g = (bobhash[i + d]->run(str, strlen(str))) % 2;
            int sign_bits = get_signbits(sign_counter[i][index[i]]);
	    int used_bits = COUNTER_SIZE - l_sign - sign_bits;
	    int borrowed_bits = get_signbits(sign_counter[i][index[i] + 1]);
	    LL value = get_val(sign_counter[i][index[i]]);

            if(g == 0)
            {           
                if(value == ((1 << used_bits) - 1)){
			bool m = add_next(sign_counter[i][index[i] + 1], sign_counter[i][index[i] + 2]);
			if(m == true){//borrow once
				after_borrow(sign_counter[i][index[i]]);
			}
			else if(m == false){//again borrow
				bool n = again_borrow(sign_counter[i][index[i] + 2], sign_counter[i][index[i] + 1], sign_counter[i][index[i] + 3]);
				if(n == true){
					after_again_borrow(sign_counter[i][index[i] + 1]);
					after_borrow(sign_counter[i][index[i]]);
				}
				else if(n == false){
					after_again_borrow(sign_counter[i][index[i] + 1]);
					after_borrow(sign_counter[i][index[i]]);
				}
			}	
		}
		else {
			add_one(sign_counter[i][index[i]]);
		}
            }
            else
            {
                if(value == 0 && borrowed_bits != 0){
			bool m = minus_next(sign_counter[i][index[i] + 1]);
			if(m == true){//borrow once
				after_minus(sign_counter[i][index[i]]);
			}
               }
               else {
			minus_one(sign_counter[i][index[i]]);
		}
	   }

        }
    }
    //the query function for borrow
	int borrow_query(const char *str) {
		int temp;
		int res[20];
		int g;
		for (int i = 0; i < d; i++)
		{
			index[i] = (bobhash[i]->run(str, strlen(str))) % w;
			if(get_signbits(sign_counter[i][index[i] + 1]) == 0){
				temp = get_val(sign_counter[i][index[i]]);
			}
			else{
				int used_bits = COUNTER_SIZE - l_sign -  get_signbits(sign_counter[i][index[i]]);
				temp = (get_borrow_val(sign_counter[i][index[i] + 1]) << (used_bits))+ get_val(counter[i][index[i]]);
			}
			g = (bobhash[i + d]->run(str, strlen(str))) % 2;
			res[i] = (g == 0 ? temp : -temp);
		}
		sort(res, res + d);
		int r;
		if (d % 2 == 0)
		{
			r = (res[d / 2] + res[d / 2 - 1]) / 2;
		}
		else
		{
			r = res[d / 2];
		}
		return r;
	}

   
	~CSketch()
	{
		for(int i = 0; i < d; i++)	
		{
			delete []counter[i];
			delete [] sign_counter[i];
		}
		for(int i = 0; i < d * 2; i++)
		{
			delete bobhash[i];
		}
	}
};
 // CSKETCH_H
