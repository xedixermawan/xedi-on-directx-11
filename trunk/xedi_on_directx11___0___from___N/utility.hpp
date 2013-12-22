#pragma once

#include "stdafx.h"

unsigned char* GetByteArrayFromFile(std::string filename,unsigned int& length)  {
	unsigned char* f_bytearray = NULL;
	// open the file
	std::ifstream file_target(filename, std::ios::in | std::ios::binary | std::ios::ate);

	// if open was successful
	bool is_open_ok = file_target.is_open() ;
	assert( is_open_ok ) ;
	if(is_open_ok) {
		// find the length of the file
		int f_length = (int)file_target.tellg();
		length = f_length;
		// collect the file data
		f_bytearray = new unsigned char[f_length];
		file_target.seekg(0, std::ios::beg);
		file_target.read(reinterpret_cast<char*>(f_bytearray), f_length);
		file_target.close();
	}
	return f_bytearray;
}