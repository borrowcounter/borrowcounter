#include <iostream>
#include <math.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<string>
#include <unordered_map>
#include <fstream>
#include "borrow.h" 
#include "cmsketch.h"
#include "cusketch.h"
#include "csketch.h"
#include "bobhash.h"
#include "exp.h" 
#include "sketch.h"
#include <limits>
#include <time.h>
#include <chrono>
 
using namespace std;



int main() 
{
		printf("dataset = 'CAIDA'\n"); 
		int sketch0, sketch1, sketch2, sketch3, sketch4, sketch5, sketch6, sketch7, arr;
		double start, ends;
		cout << "type 0~5 for 'CM', 'BORROWCM','CU','BORROWCU' ,'C'or 'BORROWC' sketches" << endl;
		sketch0 = 0;  
		sketch1 = 1;
	 	sketch2 = 2;  
		sketch3 = 3;
		sketch4 = 4;  
		sketch5 = 5;
		cout << "number of arrays in sketch is 4" << endl;
		arr = 4;
		cout << "the range of memory usage(in KB) from 200 to 1000" << endl;
	    start = 200; 
		ends = 1000; 
		double err1, err2, err3, err4;
		long double sig1, sig2, sig3, sig4;
		ofstream file;
		cout << "type in the file name your data will output(including '.txt')" << endl;
		string file_name;
		cin >> file_name;
		file.open("../output/" + file_name);
		file << "memory(in KB)     ARE     sigma     AAE     sigma     ele_are      sig     ele_aae     sig" << endl;
		for (int i = start; i <= ends; i += (ends - start) / 4) {
			CAIDA_experiment(3000000.0, i, arr, err1 , sig1, sketch1, err2, sig2, err3, sig3, err4, sig4);
			file << i << "  " << err1 << "  " << sig1 << "  " << err2 << "  " << sig2 << "  " << err3 << "  " << sig3 << "  " << err4 << "  " << sig4 << endl;
		}
	
	return 0;
}

