#pragma once
#include "Error.h"
#include <string>
#include <algorithm>
#include <cctype>
#include "Tables.h"


TokenType EtoT(ExpandedToken et)
{
	if(et == ExpandedToken::FUNDAMENTALS || et == ExpandedToken::TYPE)
	{
		return TokenType::KEYWORD;
	}
	else if(et == ExpandedToken::BINARY || et == ExpandedToken::UNARY)
	{
		return TokenType::OPERATOR;
	}
	else if(et == ExpandedToken::SEPARATOR)
	{
		return TokenType::SEPARATOR;
	}
	else if(et == ExpandedToken::IDENTIFIER)
	{
		return TokenType::IDENTIFIER;
	}
	else if(et == ExpandedToken::CONSTANT)
	{
		return TokenType::CONSTANT;
	}
	else
	{
		return TokenType::UNEXPECTED;
	}

}


std::string expandedTokenToString(ExpandedToken t)
{
	switch (t)
	{
		case ExpandedToken::FUNDAMENTALS: return "fundamentals";
		case ExpandedToken::TYPE:         return "type";
		case ExpandedToken::BINARY:       return "binary operator";
		case ExpandedToken::UNARY:        return "unary operator";
		case ExpandedToken::SEPARATOR:    return "separator";
		case ExpandedToken::IDENTIFIER:    return "identifier";
		case ExpandedToken::CONSTANT:    return "constant";
		default:                          return "unknown";
	}
}

std::string to_string(const LocalExpectation &expec)
{
	std::string res;
	for (auto it = expec.expected.begin(); it != expec.expected.end(); ++it)
	{
		res += expandedTokenToString(*it);
		if (std::next(it) != expec.expected.end())
			res += ", ";
	}
	return res;
}