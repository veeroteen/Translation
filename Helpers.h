#pragma once
#include "Error.h"
#include <string>
#include <algorithm>
#include <cctype>
#include "Tables.h"
bool isKeyword(ExpandedToken t)
{
	return t == ExpandedToken::FUNDAMENTALS ||
		t == ExpandedToken::TYPE;
}

bool isOperator(ExpandedToken t)
{
	return t == ExpandedToken::BINARY ||
		t == ExpandedToken::UNARY;
}

bool isSeparator(ExpandedToken t)
{
	switch (t)
	{
		case ExpandedToken::COMMA:
		case ExpandedToken::SEMICOLON:
		case ExpandedToken::OPEN_PAREN:
		case ExpandedToken::CLOSE_PAREN:
		case ExpandedToken::OPEN_BRACE:
		case ExpandedToken::CLOSE_BRACE:
		case ExpandedToken::OPEN_BRACKET:
		case ExpandedToken::CLOSE_BRACKET:
			return true;
		default:
			return false;
	}
}


bool isConstant(ExpandedToken t)
{
	return t == ExpandedToken::CONSTANT;
}

bool isIdentifier(ExpandedToken t)
{
	return t == ExpandedToken::IDENTIFIER_EX || t == ExpandedToken::IDENTIFIER_UNEX;
}


std::string expandedTokenToString(ExpandedToken t)
{
	switch (t)
	{
		case ExpandedToken::FUNDAMENTALS: return "fundamentals";
		case ExpandedToken::TYPE:         return "type";
		case ExpandedToken::BINARY:       return "binary operator";
		case ExpandedToken::UNARY:        return "unary operator";
		case ExpandedToken::COMMA:        return "comma";
		case ExpandedToken::SEMICOLON:    return "semicolon";
		case ExpandedToken::OPEN_PAREN:   return "open_paren";
		case ExpandedToken::CLOSE_PAREN:  return "close_paren";
		case ExpandedToken::OPEN_BRACE:   return "open_brace";
		case ExpandedToken::CLOSE_BRACE:  return "close_brace";
		case ExpandedToken::OPEN_BRACKET: return "open_bracket";
		case ExpandedToken::CLOSE_BRACKET:return "close_bracket";
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