/*
 * msquebasetest.cpp
 *  Created on: 2013-8-17
 *	    Author: qianqians
 * msque»ù×¼²âÊÔ
 */
#include <msque.h>
#include <base_test.h>

int main(){
	base_test<lock_free::msque<test> > test;
	test(10);

	return 1;
}