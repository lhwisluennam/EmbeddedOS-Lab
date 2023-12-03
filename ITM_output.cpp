/*
 * ITM_output.cpp
 *
 *  Created on: 13 Nov 2023
 *      Author: luennam
 */



#include "ITM_output.h"
#include "ITM_write.h"
#include <string>

using namespace std;


ITM_output::ITM_output() {
	// TODO Auto-generated constructor stub
	ITM_init();

}

ITM_output::~ITM_output() {
	// TODO Auto-generated destructor stub
}

//function to print 'C style string'
void ITM_output::print(const char *str){
	ITM_write(str);
}


//function to print 'C++ style string'
void ITM_output::print(const string &str){
	const char *cStyleStr = str.c_str();
	ITM_write(cStyleStr);
}

//function to print integer
void ITM_output::print(int i){
	string num = to_string(i);
	const char *cStyleStr = num.c_str();
	ITM_write(cStyleStr);
}
