//Error.h
#pragma once
#include <set>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <iostream>

struct LocalExpectation
{
	ExpandedToken expectator;
	size_t n;
	std::set<ExpandedToken> expected;
	template <typename... Args>
	LocalExpectation(size_t n,ExpandedToken expectator, Args... args) : n(n),expectator(expectator)
	{
		(expected.insert(args), ...);
	}
};

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