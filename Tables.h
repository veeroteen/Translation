#pragma once
#include <set>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <iostream>

enum class  ExpandedToken
{
   FUNDAMENTALS, // for,if, etc..
   TYPE, // int, void, etc...
   BINARY, // + - etc...
   UNARY, // ++ -- etc...
   COMMA,        // ,
   SEMICOLON,    // ;

   OPEN_PAREN,   // (
   CLOSE_PAREN,  // )

   OPEN_BRACE,   // {
   CLOSE_BRACE,  // }

   OPEN_BRACKET, // [
   CLOSE_BRACKET, // ]
   IDENTIFIER_EX,
   IDENTIFIER_UNEX,
   CONSTANT,
   UNEXPECTED
};

enum class  TokenType
{
    //dynamic
    IDENTIFIER,
    CONSTANT,

    //static
    KEYWORD,
    OPERATOR,
    SEPARATOR,

    //default
    DEFAULT
};



enum class  IdentificatorType
{
    VARIABLE,
    FUNCTION
};
enum class  ValueType
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
    TokenType type;
    size_t i;
    Lexeme(TokenType type, size_t i) : type(type), i(i) {};
};


struct Fundamentals
{
   std::string lexeme;
   ExpandedToken type;
   TokenType tokenType;
   Fundamentals(std::string &lexeme, TokenType token, ExpandedToken type) : lexeme(lexeme), type(type), tokenType(token) {};

   bool operator >(Fundamentals &a)
   {
      return lexeme > a.lexeme;
   }
   bool operator <(Fundamentals &a)
   {
      return lexeme < a.lexeme;
   }
};

class StaticTable
{
    std::vector<Fundamentals> lexems;
public:

    StaticTable() = default;
    void load(std::vector<Fundamentals>&& _lexemes)
    {
        lexems = std::move(_lexemes);
        std::sort(lexems.begin(), lexems.end());
    }
    size_t find(const std::string& lex)
    {
        size_t left = 0;
        size_t right = lexems.size() - 1;
        while (left <= right)
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
    Fundamentals getRaw(size_t i)
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
};



class DynamicTable
{
    std::unordered_map<std::string, size_t> identificators;
    std::vector<Fundamentals> IList;
    std::vector<LexemeAttributes*> attributes;
public:
    DynamicTable() = default;
    size_t add(std::string& strlex,TokenType token, ExpandedToken type, LexemeAttributes *lexema = nullptr)
    {
        if (identificators.find(strlex) == identificators.end())
        {
            size_t i = IList.size();
            IList.emplace_back(strlex,token,type);
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
        return IList[i].lexeme;
    }
    Fundamentals getRaw(size_t i)
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
    void setAttribute(size_t n,LexemeAttributes *lexema)
    {
      
    }
};