#pragma once
#include <set>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <iostream>
enum TokenType
{
   //dynamic
   IDENTIFIER,
   CONSTANT,

   //static
   KEYWORD,
   OPERATOR,
   SEPARATOR
};

enum IdentificatorType
{
   VARIABLE,
   FUNCTION
};
enum ValueType
{
   NONE,
   INT,
   FLOAT,
   CHAR,
};

struct LexemeAttributes
{
   ValueType type;
   void *value;
   LexemeAttributes(size_t i, ValueType type = NONE, void *value = nullptr) :type(type),value(value) {};
};

struct Lexeme
{
   TokenType type;
   size_t i;
   Lexeme(TokenType type, size_t i) : type(type), i(i) {};
};


class StaticTable
{
   std::vector<std::string> lexems;
public:
   
   StaticTable() = default;
   void load(std::vector<std::string> &&_lexemes)
   {
      lexems = std::move(_lexemes);
      std::sort(lexems.begin(), lexems.end());
   }
   size_t find(const std::string &lexeme)
   {
      size_t left = 0;
      size_t right = lexeme.size() - 1;
      while(left <= right)
      {
         size_t cur = (left + right) / 2;
         if(lexems[cur] == lexeme)
         {
            return cur;
         }
      
         if(lexems[cur] < lexeme)
         {
            left = cur + 1;
         }
         else
         {
            right = cur - 1;
         }
      }
      return lexems.size();
   }
   bool contains(const std::string &lexeme)
   {
      if(find(lexeme) == lexems.size())
      {
         return false;
      }
      return true;
   }
};



class DynamicTable
{
   std::unordered_map<std::string, size_t> identificators;
   std::vector<std::string> IList;
   std::vector<LexemeAttributes> attributes;
public:
   DynamicTable() = default;
   size_t add(std::string &strlex, LexemeAttributes lexema)
   {
      size_t i = IList.size();
      IList.push_back(strlex);
      attributes.push_back(lexema);
      identificators[strlex] = i;
      return i;
   }
   auto find(std::string &lex)
   {
      return identificators.find(lex);
   }

};