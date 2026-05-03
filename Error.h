//Error.h
#pragma once
#include <set>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <iostream>



class Error
{
public:
	static std::vector<std::string> errors;

	static void add(std::string &error)
	{
		errors.push_back(error);
	}
	static void printErrors(std::ostream &stream)
	{
		for(auto &a : errors)
		{
			stream << a << std::endl;
		
		}
	}
};