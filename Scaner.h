#pragma once
#include "Tables.h"
#include <fstream>
#include "MISC.h"
#include "Error.h"
#include <list>
#include "Helpers.h"

class Scanner
{
   StaticTable keywords;
   StaticTable operators;
   StaticTable separators;
   DynamicTable constants;
   DynamicTable identificators;
   std::vector<Lexeme> lexemes;
   std::set<char> forbiddenIName;

   Fundamentals findInTable(std::string &word, LocalExpectation expec)
   {
      for(auto &a : expec.expected)
      {
         if (isKeyword(a))
         {
            size_t i = keywords.find(word);
            if (i != keywords.size())
            {
               return keywords.getRaw(i);
            }
         }
         else if (isOperator(a))
         {
            size_t i = operators.find(word);
            if (i != operators.size())
            {
               return operators.getRaw(i);
            }
         }
         else if (isSeparator(a))
         {
            size_t i = separators.find(word);
            if (i != separators.size())
            {
               return separators.getRaw(i);
            }
         }
         else if (isIdentifier(a))
         {
            size_t i = separators.find(word);
            if (i != separators.size())
            {
               return separators.getRaw(i);
            }
         }
         else if (isConstant(a))
         {
            size_t i = separators.find(word);
            if (i != separators.size())
            {
               return separators.getRaw(i);
            }
         }
      }
      std::string buff = "";
      return Fundamentals(buff, TokenType::DEFAULT, ExpandedToken::UNEXPECTED);
   
   
   }
   void makeError(size_t strn, LocalExpectation &localExpectation)
   {
      std::string error = "line: " + std::to_string(strn) + ", expected: " + to_string(localExpectation);
      Error::errors.push_back(error);
   }
   void scan(std::string &codePath)
   {
      std::ifstream code(codePath);
      size_t strn = 1;
      while (!code.eof())
      {
         std::string str = "";
         std::getline(code, str);
         trim(str);
         auto start = str.begin();
         size_t level = 0;
         LocalExpectation localExpectation(lexemes.size(), ExpandedToken::FUNDAMENTALS, ExpandedToken::TYPE);
         while (start!= str.end())
         {
            auto separ = findSep(start, str.end(), forbiddenIName);

            
            std::string word = std::string(start, separ); // space ?
            Fundamentals lex = findInTable(word,localExpectation);
            
            if(lex.type == ExpandedToken::UNEXPECTED)
            {
               if(localExpectation.expected.contains(ExpandedToken::IDENTIFIER_UNEX))
               { 
                  if(isIdentifier(word))
                  {
                     if(localExpectation.expectator == ExpandedToken::TYPE)
                     {
                        
                     }
                  
                  }
               }
               else if(localExpectation.expected.contains(ExpandedToken::CONSTANT))
               {
                  if (isInteger(word))
                  {


                  }
               }
               else
               {
                  makeError(strn, localExpectation);
                  return;
               }
               continue;
            }
            switch (lex.tokenType)
            {
               case TokenType::KEYWORD:
               {
                  lexemes.emplace_back(lex.tokenType, keywords.find(word));
                  switch (lex.type)
                  {
                     case ExpandedToken::FUNDAMENTALS:
                     {
                        break;
                     }
                     case ExpandedToken::TYPE:
                     {
                        localExpectation = LocalExpectation(lexemes.size() - 1, lex.type, ExpandedToken::IDENTIFIER_UNEX, ExpandedToken::OPEN_PAREN);
                        break;
                     }
                  }
                  break;
               }
               case TokenType::IDENTIFIER:
               {
                  break;
               }
               case TokenType::DEFAULT:
               {

               }
            }

            
            start = separ+1;
         }
      }
   
   }


public:
	Scanner(std::string &testFolder)
	{
      testFolder += "/";
      

      {
         std::ifstream stream(testFolder + "Keywords.txt");
         std::string buff = "";
         std::vector<Fundamentals> bArr;
         while (std::getline(stream, buff))
         {
            std::pair<std::string, ExpandedToken> par;
            divide(par, buff);
            bArr.emplace_back(par.first,  TokenType::KEYWORD, par.second);
         }
         keywords.load(std::move(bArr));
      }

      {
         std::ifstream stream(testFolder + "Operators.txt");
         std::string buff = "";
         std::vector<Fundamentals> bArr;
         while (std::getline(stream, buff))
         {
            std::pair<std::string, ExpandedToken> par;
            divide(par, buff);

            bArr.emplace_back(par.first, TokenType::OPERATOR, par.second);
            for (auto &a : par.first)
            {
               forbiddenIName.insert(a);
            }
         }
         operators.load(std::move(bArr));
      }

      {
         std::ifstream stream(testFolder + "Separators.txt");
         std::string buff = "";
         std::vector<Fundamentals> bArr;
         while (std::getline(stream, buff))
         {
            std::pair<std::string, ExpandedToken> par;
            divide(par, buff);
            bArr.emplace_back(par.first, TokenType::SEPARATOR, par.second);
            for (auto &a : par.first)
            {
               forbiddenIName.insert(a);
            }
         }
         separators.load(std::move(bArr));
      }
      forbiddenIName.insert(' ');

      std::string codePath(testFolder + "main.cpp");
      scan(codePath);
	}


};