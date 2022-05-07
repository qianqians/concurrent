/*
 * thread_grouptest.cpp
 *  Created on: 2021-7-30
 *	    Author: qianqians
 * thread_group test
 */

#include <string_tools.h>

int main(){
	auto test = ",1,2,3,";
	auto v_str_split = concurrent::split(test, ",");
	auto v_str_split1 = concurrent::split(test, ',');

	auto wtest = L"1,2,3,";
	auto v_wstr_split = concurrent::split(wtest, L",");
	auto v_wstr_split1 = concurrent::split(wtest, L',');

	auto u8test = u8"1,2,3,";
	auto v_u8str_split = concurrent::split(u8test, u8",");
	auto v_u8str_split1 = concurrent::split(u8test, u8',');

	auto u16test = u"1,2,3,";
	auto v_u16str_split = concurrent::split(u16test, u",");
	auto v_u16str_split1 = concurrent::split(u16test, u',');

	auto u32test = U"1,2,3,";
	auto v_u32str_split = concurrent::split(u32test, U",");
	auto v_u32str_split1 = concurrent::split(u32test, U',');

	return 1;
}