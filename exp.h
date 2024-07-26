#pragma once

#include <iostream>
#include <math.h>
#include "sketch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>
#include <fstream>
#include "borrow.h"
#include "cmsketch.h"
#include "cusketch.h"
#include "csketch.h"
#include "bobhash.h"
#include <limits>
#include <time.h>
#include <chrono>

using namespace std;

char insert[30000000 + 30000000 / 5][15];

bool comp(pair<string, int> &a1, pair<string, int> &a2)
{
	return a1.second < a2.second;
}

void CAIDA_experiment(int number, double mem, int arr, double &ret_error, long double &sigma, int sketch, double &ret_aae, long double &aae_sig, double &ret_ele_error, long double &ele_error_sig, double &ret_ele_aae, long double &ele_aae_sig)
{
	cout << "your sketch number is: " << sketch << " (0~5 for 'CM', 'BORROWCM','CU', 'BORROWCU','C', or 'BORROWC' sketches)" << endl;
	class_sketches *s[20];
	if (sketch == 0)
	{

		for (int i = 0; i < LOOP_NUM; ++i)
		{
			s[i] = new CMSketch(mem * 1024 / 4 / arr, arr);
		}
		cout << "Bucket nums " << mem * 1024 *8 / 32 << endl;
		cout << "CMSketch with " << mem << "KB hash memory generated!" << endl;
	}
	if (sketch == 2)
	{
		for (int i = 0; i < LOOP_NUM; ++i)
		{
			s[i] = new CUSketch(mem * 1024 / 4 / arr, arr);
		}
		cout << "Bucket nums " << mem * 1024 *8 / 32 << endl;
		cout << "CUSketch with " << mem << "KB hash memory generated!" << endl;
	}
	if (sketch == 4)
	{
		for (int i = 0; i < LOOP_NUM; ++i)
		{
			s[i] = new CSketch(mem * 1024 / 4 / arr, arr);
		}
		cout << "Bucket nums " << mem * 1024 *8 / 32 << endl;
		cout << "CSketch with " << mem << "KB hash memory generated!" << endl;
	}
	if (sketch == 1)
	{
		for (int i = 0; i < LOOP_NUM; ++i)
		{
			s[i] = new CMSketch(mem * 1024 * 8 / COUNTER_SIZE / arr, arr);
		}
		cout << "Bucket nums " << mem * 1024 * 8 / COUNTER_SIZE  << endl;
		cout << "BORROW CMSketch with " << mem << "KB hash memory generated!" << endl;
	}
	if (sketch == 3)
	{
		for (int i = 0; i < LOOP_NUM; ++i)
		{
			s[i] = new CUSketch(mem * 1024 * 8 / COUNTER_SIZE / arr, arr);
		}
		cout << "Bucket nums " << mem * 1024 * 8 / COUNTER_SIZE  << endl;
		cout << "BORROW CUSketch with " << mem << "KB hash memory generated!" << endl;
	}
	if (sketch == 5)
	{
		for (int i = 0; i < LOOP_NUM; ++i)
		{
			s[i] = new CSketch(mem * 1024 * 8 / COUNTER_SIZE / arr, arr);
		}
		cout << "Bucket nums " << mem * 1024 * 8 / COUNTER_SIZE  << endl;
		cout << "BORROW CSketch with " << mem << "KB hash memory generated!" << endl;
	}
	
	unordered_map<string, int> unmp[20];

	int package_num = 0;
	int total_size;
	double error[20];
	double error_aae[20];
	double ele_error[20];
	double ele_error_aae[20];
	double mic_error[20];
	double mic_error_aae[20];
	char filename[55];
	int x=0;
	memset(filename, 0, sizeof(char) * 55);

	sprintf(filename, "../../dataset/2019.dat");
	//  
	FILE *file_stream = fopen(filename, "rb");
	double time_count = 0;
	double time_count2 = 0;

	double flow_num_sum = 0;
	for (int m = 0; m < LOOP_NUM; ++m)
	{
		package_num = 0;
		total_size = 0;

		char buf[15];

		while (fread(insert[package_num], 13, 1, file_stream) && total_size < number)
		{
			string s(insert[package_num++], 13);
			unmp[m][s]++;
			total_size++;
		}

		cout << "total size=" << total_size << endl;

		int max_freq = 0;
		unordered_map<string, int>::iterator it = unmp[m].begin();

		for (int i = 0; i < unmp[m].size(); i++, it++)
		{

			int temp2 = it->second;

			max_freq = max_freq > temp2 ? max_freq : temp2;
		}

		cout<<"loading complete and max_freq="<<max_freq<<endl;
		cout<<"total size="<<total_size<<endl;
		cout<<"package_number="<<package_num<<endl;
		printf("unmp[%d].size: %d\n",m,unmp[m].size());
		unordered_map<string, int>::iterator it1;
		auto start = chrono::system_clock::now();

		if (sketch == 0 || sketch == 2 ||sketch == 4 ||sketch == 6)
			for (int i = 0; i < package_num; ++i)
				s[m]->Insert(insert[i], 1);
		else if (sketch ==1 || sketch == 3 ||sketch == 5 ||sketch == 7)
			for (int i = 0; i < package_num; ++i)
				s[m]->borrow_insert(insert[i], 1);

		auto end = chrono::system_clock::now();
		auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
		time_count += double(duration.count()) * chrono::microseconds::period::num / chrono::microseconds::period::den;

		int flow_num = unmp[m].size();
		flow_num_sum += flow_num;
		

		unordered_map<string, int>::iterator it2;

		error[m] = 0;
		error_aae[m] = 0;
		ele_error[m] = 0;
		ele_error_aae[m] = 0;
		mic_error[m] = 0;
		mic_error_aae[m] = 0;

		start = chrono::system_clock::now();

		if (sketch == 0 || sketch == 2 ||sketch == 4 ||sketch == 6){
			for (it2 = unmp[m].begin(); it2 != unmp[m].end(); ++it2){
				s[m]->Query(it2->first.c_str());
			}
		}
		else if (sketch ==1 || sketch == 3 ||sketch == 5 ||sketch == 7)
			for (it2 = unmp[m].begin(); it2 != unmp[m].end(); ++it2)
				s[m]->borrow_query(it2->first.c_str());

		end = chrono::system_clock::now();
		duration = chrono::duration_cast<chrono::microseconds>(end - start);
		time_count2 += double(duration.count()) * chrono::microseconds::period::num / chrono::microseconds::period::den;

		int ele_flow_num = 0;
		int mic_flow_num = 0;
		int c = 0;
		
		vector<pair<string, int>> flows_num_vec(unmp[m].begin(), unmp[m].end());
	        sort(flows_num_vec.begin(), flows_num_vec.end(), comp);
		
		for (auto it2 = flows_num_vec.begin(); it2 != flows_num_vec.end(); ++it2)
		{
			if (sketch == 0 || sketch == 2 ||sketch == 4 ||sketch == 6)
			{
				error[m] += abs(s[m]->Query(it2->first.c_str()) - it2->second) / (double)it2->second;
				error_aae[m] += abs((double)s[m]->Query(it2->first.c_str()) - it2->second);
				if(c++ < 0.99 * flow_num){
					mic_flow_num++;
					mic_error[m] += abs(s[m]->Query(it2->first.c_str()) - it2->second) / (double)it2->second;
					mic_error_aae[m] += abs(s[m]->Query(it2->first.c_str()) - it2->second);//(double)
				}
				else{
					
					ele_flow_num++;
					ele_error[m] += abs(s[m]->Query(it2->first.c_str()) - it2->second) / (double)it2->second;
					ele_error_aae[m] += abs(s[m]->Query(it2->first.c_str()) - it2->second);//(double)
				}
			}
			else if (sketch ==1 || sketch == 3 ||sketch == 5 ||sketch == 7)
			{
				error[m] += abs(s[m]->borrow_query(it2->first.c_str()) - it2->second) / (double)it2->second;
				error_aae[m] += abs(s[m]->borrow_query(it2->first.c_str()) - it2->second);
				if(c++ < 0.99 * flow_num){
					mic_flow_num++;
					mic_error[m] += abs(s[m]->borrow_query(it2->first.c_str()) - it2->second) / (double)it2->second;
					mic_error_aae[m] += abs((double)s[m]->borrow_query(it2->first.c_str()) - it2->second);
				}
				else{
					ele_flow_num++;
					ele_error[m] += abs(s[m]->borrow_query(it2->first.c_str()) - it2->second) / (double)it2->second;
					ele_error_aae[m] += abs((double)s[m]->borrow_query(it2->first.c_str()) - it2->second);
				}
			}			
		}

		error[m] /= flow_num;
		error_aae[m] /= flow_num;
		ele_error[m] /= ele_flow_num;
		ele_error_aae[m] /= ele_flow_num;
		mic_error[m] /= mic_flow_num;
		mic_error_aae[m] /= mic_flow_num;
		
	}
	fclose(file_stream);

	ret_error = 0;
	sigma = 0;
	ret_aae = 0;
	aae_sig = 0;
	ret_ele_error = 0;
	ele_error_sig = 0;
	ret_ele_aae = 0;
	ele_aae_sig = 0;
	
	for (int m = 0; m < LOOP_NUM; ++m)
	{
		ret_error += error[m];
		ret_aae += error_aae[m];
		ret_ele_error += ele_error[m];
		ret_ele_aae += ele_error_aae[m];
		cout << "are is " << error[m] << endl;
		cout << "aae is " << error_aae[m] << endl;
		
		cout << "ele_are is " << ele_error[m] << endl;
		cout << "ele_aae is " << ele_error_aae[m] << endl;
		cout << "mic_are is " << mic_error[m] << endl;
		cout << "mic_aae is " << mic_error_aae[m] << endl;
		cout << "*************************"<<endl;
	}

	printf("throughput (insert)_for_sketch_number_%d:%f\n", sketch, total_size * LOOP_NUM / (time_count * 1000000));

	printf("throughput (query)_for_sketch_number_%d:%f\n", sketch, flow_num_sum / (time_count2 * 1000000));
	

	ret_error /= LOOP_NUM;
	ret_aae /= LOOP_NUM;
	ret_ele_error /= LOOP_NUM;
	ret_ele_aae /= LOOP_NUM;
	for (int m = 0; m < LOOP_NUM; ++m)
	{
		sigma += pow(error[m] - ret_error, 2);
		aae_sig += pow(error_aae[m] - ret_aae, 2);
		ele_error_sig += pow(ele_error[m] - ret_ele_error, 2);
		ele_aae_sig += pow(ele_error_aae[m] - ret_ele_aae, 2);
	}
	sigma = sqrt(sigma / (LOOP_NUM - 1));
	aae_sig = sqrt(aae_sig / (LOOP_NUM - 1));
	ele_error_sig = sqrt(ele_error_sig / (LOOP_NUM - 1));
	ele_aae_sig = sqrt(ele_aae_sig / (LOOP_NUM - 1));
}


