/*
 * ITM_output.h
 *
 *  Created on: 13 Nov 2023
 *      Author: luennam
 */

#ifndef ITM_OUTPUT_H_
#define ITM_OUTPUT_H_

#include <string>
using namespace std;

class ITM_output {
public:
	ITM_output();
	virtual ~ITM_output();
	void print(const char *str);		//function to print 'C style string'
	void print(const string &str);		//function to print 'C++ style string'
	void print(int i);					//function to print integer

};

#endif /* ITM_OUTPUT_H_ */
