#pragma once

#include <string>
#include <sstream>
#include <windows.h>

using namespace std;

namespace CJen
{
	inline void convertToCharArr(char arr[], std::string msg)
	{
		for(int i = 0; i < (int)msg.length(); i++)
			arr[i] = msg[i];
	}

	inline void convertFromCharArr(char arr[], string& msg)
	{
		msg = "";
		for(int i = 0; i < arr[i] != 0 && i < 512; i++)
			msg += arr[i];
	}

	inline std::string convertNrToString(float nr)
	{
		std::string s;
		std::stringstream out;
		out << nr;
		s = out.str();
		return s;
	}

	template<typename T>
	inline void addNrToString(const T &nr, std::string &out)
	{
		std::stringstream output;
		output << nr;
		out += output.str();
	}
}
