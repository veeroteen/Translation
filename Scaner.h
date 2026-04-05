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
   std::vector<LexemeI> lexemes;
   std::set<char> stringSep;

   std::string getLexemeStr(LexemeI &lex)
   {
      TokenType table = EtoT(lex.type);
      switch(table)
      {
         case TokenType::KEYWORD:
         {
            return keywords.getVal(lex.i);
            break;
         }
         case TokenType::OPERATOR:
         {
            return operators.getVal(lex.i);
            break;
         }
         case TokenType::SEPARATOR:
         {
            return separators.getVal(lex.i);
            break;
         }
         case TokenType::IDENTIFIER:
         {
            return identificators.getVal(lex.i);
            break;
         }
         case TokenType::CONSTANT:
         {
            return constants.getVal(lex.i);
            break;
         }
      }
   }

   void makeError(size_t strn, LocalExpectation &localExpectation)
   {
      std::string error = "line: " + std::to_string(strn) + ", expected: " + to_string(localExpectation);
      Error::errors.push_back(error);
   }
   void makeError(size_t strn, std::string str)
   {
      std::string error = "line: " + std::to_string(strn) + ", " + str;
      Error::errors.push_back(error);
   }
   int addLexeme(std::string &word)
   {
      size_t i = keywords.find(word);
      if(i != keywords.size())
      {
         lexemes.emplace_back(keywords.getRaw(i).type,i);
         return 0;
      }
      i = operators.find(word);
      if (i != operators.size())
      {
         lexemes.emplace_back(operators.getRaw(i).type,i);
         return 0;
      }
      i = separators.find(word);
      if (i != separators.size())
      {
         lexemes.emplace_back(separators.getRaw(i).type,i);
         return 0;
      }
   
      if (isIdentifier(word))
      {
         size_t i = identificators.find(word);
         if (i == identificators.size())
         {
            i = identificators.add(word, ExpandedToken::IDENTIFIER);
         }
         lexemes.emplace_back(identificators.getRaw(i).type,i);
         return 0;
      }
      if (isInteger(word))
      {
         size_t i = constants.find(word);
         if (i == constants.size())
         {
            i = constants.add(word, ExpandedToken::IDENTIFIER);
         }
         lexemes.emplace_back(constants.getRaw(i).type,i);
         return 0;
      }
      return 1;

   }

   void lexscan(std::string &codePath)
   {
      std::ifstream code(codePath);
      size_t strn = 1;
      while (!code.eof())
      {
         std::string str = "";
         std::getline(code, str);
         trim(str);
         auto start = str.begin();

         while (start!= str.end())
         {
            auto separ = findSep(start, str.end(), stringSep);
            std::string word = std::string(start, separ);
            int err = addLexeme(word);
            if(err != 0 && *separ == ' ')
            {
               makeError(strn, word + "  user-defined literal not found or unappropriate");
            
            }
            if(*separ != ' ')
            {
               word = std::string(separ, separ+1);
               err = addLexeme(word);
               if (err != 0)
               {
                  makeError(strn, word + "  user-defined literal not found or unappropriate");

               }
            }
            start = separ+1;
         }
         strn += 1;
      }
   
   }


public:
   Scanner(std::string &testFolder)
	{
      testFolder += "/";
      

      {
         std::ifstream stream(testFolder + "Keywords.txt");
         std::string buff = "";
         std::vector<Lexeme> bArr;
         while (std::getline(stream, buff))
         {
            std::pair<std::string, ExpandedToken> par;
            divide(par, buff);
            bArr.emplace_back(par.first, par.second);
         }
         keywords.load(std::move(bArr));
      }

      {
         std::ifstream stream(testFolder + "Operators.txt");
         std::string buff = "";
         std::vector<Lexeme> bArr;
         while (std::getline(stream, buff))
         {
            std::pair<std::string, ExpandedToken> par;
            divide(par, buff);

            bArr.emplace_back(par.first, par.second);
         }
         operators.load(std::move(bArr));
      }

      {
         std::ifstream stream(testFolder + "Separators.txt");
         std::string buff = "";
         std::vector<Lexeme> bArr;
         while (std::getline(stream, buff))
         {
            std::pair<std::string, ExpandedToken> par;
            divide(par, buff);
            bArr.emplace_back(par.first, par.second);
            for (auto &a : par.first)
            {
               stringSep.insert(a);
            }
         }
         separators.load(std::move(bArr));
      }
      stringSep.insert(' ');

      std::string codePath(testFolder + "main.cpp");
      lexscan(codePath);
      std::ofstream out("lexemes.txt");
      for(auto &a : lexemes)
      {
         out << getLexemeStr(a) << "  ( " << expandedTokenToString(a.type) << ", " << a.i << " );\n";
      }
      out.close();
      out.open("errors.txt");
      Error::printErrors(out);
	}


};