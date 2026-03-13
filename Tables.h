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

struct functionValue
{
   ValueType value;
   std::vector<ValueType> args;

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
   void *value;
   LexemeAttributes( void *value = nullptr) : value(value) {};
   ~LexemeAttributes()
   {
      if(value != nullptr)
      {
         delete value;
      }
   }
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
   size_t find(const std::string &lex)
   {
      size_t left = 0;
      size_t right = lex.size() - 1;
      while(left <= right)
      {
         size_t cur = (left + right) / 2;
         if(lexems[cur] == lex)
         {
            return cur;
         }
      
         if(lexems[cur] < lex)
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
   size_t size()
   {
      return lexems.size();
   }
   std::string getVal(size_t i)
   {
      return lexems[i];
   }
   bool contains(const std::string &lex)
   {
      find(lex) == lexems.size() ? false : true;
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
   auto find(const std::string &lex)
   {
      return (*identificators.find(lex)).second;
   }
   std::string getVal(size_t i)
   {
      return IList[i];
   }
   LexemeAttributes getAttributes(size_t i)
   {
      return attributes[i];
   }
   bool contains(const std::string &lex)
   {
      find(lex) == identificators.size() ? false : true;
   }
};