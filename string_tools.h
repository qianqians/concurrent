/*
 * string.h
 *		 Created on: 2021-9-18
 *			 Author: qianqians
 * string
 */

#ifndef _STRING_H
#define _STRING_H    
    
#include <stdarg.h>
#include <string>
#include <vector>    

namespace concurrent {

inline std::vector<std::string> split(std::string in, std::string token)
{
	std::vector<std::string> vstr;
	do {
		if (in == "") {
			break;
		}

		while (true) {
			auto index = in.find(token);

			if (index == std::string::npos) {
				vstr.push_back(in);
				break;
			}

			vstr.push_back(in.substr(0, index));
			in = in.substr(index + token.size());
		}

	} while (false);
    
	return vstr;
}

}

#endif //_STRING_H