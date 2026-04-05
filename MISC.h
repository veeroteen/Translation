//MISC.h
#pragma once
#include <string>
#include <algorithm>
#include <cctype>
#include "Error.h"
void trim(std::string &str)
{
   auto comm = str.find("//");
   if(comm != std::string::npos)
   {
      str.erase(comm+str.begin(), str.end());
   }
   auto it = std::find_if(str.begin(), str.end(),
      [](unsigned char c)
      {
         return !std::isspace(c);
      });

   str.erase(str.begin(), it);
   str.erase
   (
      std::unique
      (
         str.begin(),
         str.end(),
         [](char a, char b)
         {
            return a == ' ' && b == ' ';
         }
      ),
      str.end()
   );

   str.erase(
      std::remove_if(str.begin(), str.end(),
         [](char c)
         {
            return std::isspace(c) && c != ' ';
         }),
      str.end()
   );
   auto rit = std::find_if(str.rbegin(), str.rend(),
      [](unsigned char c)
      {
         return !(c == ' ');
      });

   str.erase(rit.base(), str.end());

   return;
}

auto findSep(const std::string::iterator begin,const std::string::iterator end,const std::set<char> &forbidden)
{
   auto a = begin;
   for(;a != end;a++)
   {
      if(forbidden.find(*a) != forbidden.end())
      {
         return a;
      }
      
   }
}

bool isInteger(const std::string &s)
{
   if (s.empty()) return false;

   size_t i = 0;
   if (s[0] == '-' || s[0] == '+') i++;

   if (i == s.size()) return false;

   for (; i < s.size(); i++)
      if (!std::isdigit(s[i])) return false;

   return true;
}

bool isIdentifier(const std::string &s)
{
   if (s.empty()) return false;


   if (!std::isalpha(s[0]) && s[0] != '_')
      return false;

   for (size_t i = 1; i < s.size(); i++)
      if (!std::isalnum(s[i]) && s[i] != '_')
         return false;

   return true;
}

bool provide(const std::string str, ExpandedToken &out)
{
   if (str == "FUNDAMENTALS") { out = ExpandedToken::FUNDAMENTALS; return true; }
   if (str == "TYPE") { out = ExpandedToken::TYPE;         return true; }
   if (str == "BINARY") { out = ExpandedToken::BINARY; return true; }
   if (str == "UNARY") { out = ExpandedToken::UNARY;  return true; }
   if (str == "SEPARATOR") { out = ExpandedToken::SEPARATOR;         return true; }

   return false;
}

void divide(std::pair<std::string, ExpandedToken> &par,const std::string &str)
{
   auto space = std::find(str.begin(),str.end(), ' ');
   par.first = std::string(str.begin(), space);
   provide(std::string(space+1, str.end()), par.second);
}