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
    TokenType type;
    size_t i;
    Lexeme(TokenType type, size_t i) : type(type), i(i) {};
};


class StaticTable
{
    std::vector<std::string> lexems;
public:

    StaticTable() = default;
    void load(std::vector<std::string>&& _lexemes)
    {
        lexems = std::move(_lexemes);
        std::sort(lexems.begin(), lexems.end());
    }
    size_t find(const std::string& lex)
    {
        size_t left = 0;
        size_t right = lex.size() - 1;
        while (left <= right)
        {
            size_t cur = (left + right) / 2;
            if (lexems[cur] == lex)
            {
                return cur;
            }

            if (lexems[cur] < lex)
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
    bool contains(const std::string& lex)
    {
        find(lex) == lexems.size() ? false : true;
    }
    void drawTable()
    {
        for(size_t i = 0; i < lexems.size();i++)
        {
            std::cout << i << "  " << lexems[i] << std::endl;
        }
    
    }
};



class DynamicTable
{
    std::unordered_map<std::string, size_t> identificators;
    std::vector<std::string> IList;
    std::vector<LexemeAttributes*> attributes;
public:
    DynamicTable() = default;
    size_t add(std::string& strlex, LexemeAttributes *lexema)
    {
        if (identificators.find(strlex) == identificators.end())
        {
            size_t i = IList.size();
            IList.push_back(strlex);
            attributes.push_back(lexema);
            identificators[strlex] = i;
            return i;
        }
        return identificators.size();
    }
    auto find(const std::string& lex)
    {
        return identificators.find(lex);
    }
    std::string getVal(size_t i)
    {
        return IList[i];
    }
    int getAttributes( size_t i )
    {
       return *static_cast<int*>(attributes[i]->value);
    }
    bool contains(const std::string& lex)
    {
        return find(lex) == identificators.end() ? false : true;
    }
    size_t size()
    {
        return IList.size();
    }
};