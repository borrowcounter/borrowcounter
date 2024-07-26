#pragma once

//base class to make experimet function and main function simple and clear

class class_sketches {
public:
	virtual void Insert(const char * str, int c) {}
	virtual int Query(const char *str) = 0;
	virtual void borrow_insert(const char *str, int c) {}
	virtual int borrow_query(const char *str) = 0;
	virtual void int_Buckets(){}
	virtual void borrow_Buckets(){}
	virtual void check(const char* str) {}
};
