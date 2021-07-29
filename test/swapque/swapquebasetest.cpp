/*
 * swapquebasetest.cpp
 *  Created on: 2013-8-17
 *	    Author: qianqians
 * swapquebasetest»ù×¼²âÊÔ
 */
#include <swapque.h>
#include <base_test.h>

int main(){
	base_test<lock_free::swapque<test> > test;
	test(10);

	return 1;
}