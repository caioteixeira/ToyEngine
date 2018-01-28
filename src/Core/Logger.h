#pragma once
#include <iostream>

class Logger
{
public:
	static void Log(std::string str)
	{
		std::cout << str.c_str() << std::endl;
	}
};
