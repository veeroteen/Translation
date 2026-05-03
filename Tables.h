//Table.h
#pragma once
#include <set>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <iostream>

enum class ExpandedToken
{
   FUNDAMENTALS, // for,if, etc..
   TYPE, // int, void, etc...
   BINARY, // + - etc...
   UNARY, // ++ -- etc...
   SEPARATOR,
   IDENTIFIER,
   CONSTANT,
   UNEXPECTED
};

enum class TokenType
{
    //dynamic
    IDENTIFIER,
    CONSTANT,

    //static
    KEYWORD,
    OPERATOR,
    SEPARATOR,

    //
    UNEXPECTED
};

enum class IdentificatorType
{
    VARIABLE,
    FUNCTION
};
enum class ValueType
{
    VOID,
    INT,
};

struct functionValue
{
    ValueType value;
    std::vector<ValueType> args;

};

struct LexemeAttributes
{
    void* value;
    
    LexemeAttributes(void* value = nullptr) : value(value) {};

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
   std::string lexeme;
   ExpandedToken type;
   Lexeme(std::string &lexeme, ExpandedToken type) : lexeme(lexeme), type(type) {};

   bool operator >(Lexeme &a)
   {
      return lexeme > a.lexeme;
   }
   bool operator <(Lexeme &a)
   {
      return lexeme < a.lexeme;
   }
};

struct LexemeI
{
   ExpandedToken type;
   size_t i;
   size_t str_n;
   LexemeI(ExpandedToken type, size_t i,size_t str_n) : type(type), i(i), str_n(str_n) {};
};

class StaticTable
{
    std::vector<Lexeme> lexems;
public:

    StaticTable() = default;
    void load(std::vector<Lexeme>&& _lexemes)
    {
        lexems = std::move(_lexemes);
        std::sort(lexems.begin(), lexems.end());
    }
    size_t find(const std::string& lex)
    {
        size_t left = 0;
        size_t right = lexems.size() - 1;
        while (left <= right && right <  size())
        {
            size_t cur = (left + right) / 2;
            if (lexems[cur].lexeme == lex)
            {
                return cur;
            }

            if (lexems[cur].lexeme < lex)
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
        return lexems[i].lexeme;
    }
    Lexeme getRaw(size_t i)
    {
       return lexems[i];
    }
    bool contains(const std::string& lex)
    {
        return find(lex) == lexems.size() ? false : true;
    }
    void drawTable()
    {
        for(size_t i = 0; i < lexems.size();i++)
        {
            std::cout << i << "  " << lexems[i].lexeme << std::endl;
        }
    
    }
    const std::vector<Lexeme>& getTable()
    {
       return lexems;
    }
};

class DynamicTable
{
    std::unordered_map<std::string, size_t> identificators;
    std::vector<Lexeme> IList;
    std::vector<LexemeAttributes*> attributes;
public:
    DynamicTable() = default;
    size_t add(std::string& strlex, ExpandedToken type, LexemeAttributes *lexema = nullptr)
    {
        if (identificators.find(strlex) == identificators.end())
        {
            size_t i = IList.size();
            IList.emplace_back(strlex,type);
            attributes.push_back(lexema);
            identificators[strlex] = i;
            return i;
        }
        return identificators.size();
    }
    auto find(const std::string& lex)
    {
       auto a = identificators.find(lex);
       if(a == identificators.end())
       {
          return size();
       }
       else
       {
          return a->second;
       }
    }
    std::string getVal(size_t i)
    {
        return IList[i].lexeme;
    }
    Lexeme getRaw(size_t i)
    {
       return IList[i];
    }
    int getAttributes( size_t i )
    {
       return *static_cast<int*>(attributes[i]->value);
    }
    bool contains(const std::string& lex)
    {
        return find(lex) == size() ? false : true;
    }
    size_t size()
    {
        return IList.size();
    }
    void setAttribute(size_t n,LexemeAttributes *lexema)
    {
      
    }
};