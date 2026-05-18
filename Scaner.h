//Scaner.h
#pragma once
#include "Tables.h"
#include <fstream>
#include "MISC.h"
#include "Error.h"
#include <list>
#include "Helpers.h"
#include <stack>
#include "AST.h"
#include <sstream>
#include <string>
#include <iostream>

class Scanner
{
   StaticTable keywords;
   StaticTable operators;
   StaticTable separators;
   DynamicTable constants;
   DynamicTable identificators;
   Scope *global, *curScope;
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
   std::string getLexemeStr(TokenType table, size_t i)
   {

      switch (table)
      {
         case TokenType::KEYWORD:
         {
            return keywords.getVal(i);
            break;
         }
         case TokenType::OPERATOR:
         {
            return operators.getVal(i);
            break;
         }
         case TokenType::SEPARATOR:
         {
            return separators.getVal(i);
            break;
         }
         case TokenType::IDENTIFIER:
         {
            return identificators.getVal(i);
            break;
         }
         case TokenType::CONSTANT:
         {
            return constants.getVal(i);
            break;
         }
      }
   }

   void makeError(size_t strn, std::string str)
   {
      std::string error = "line: " + std::to_string(strn) + ", " + str;
      Error::errors.push_back(error);
   }
   void makeExpectEr(std::vector<LexemeI>::iterator &lex, std::string expectator)
   {
      makeError(lex->str_n,
         "Instead of " + getLexemeStr(*lex) + ", expected " + expectator);
   }
   void makeUnexEnExEr(std::vector<LexemeI>::iterator &lex)
   {
      makeError(lex->str_n,
         "Unexpected end of expression");
   }
   void makeAlExEr(std::vector<LexemeI>::iterator &lex)
   {
      makeError(lex->str_n,
         getLexemeStr(*lex) + " already exists in this scope");
   }

   int addLexeme(std::string &word,size_t str_n)
   {
      size_t i = keywords.find(word);
      if(i != keywords.size())
      {
         lexemes.emplace_back(keywords.getRaw(i).type,i, str_n);
         return 0;
      }
      i = operators.find(word);
      if (i != operators.size())
      {
         lexemes.emplace_back(operators.getRaw(i).type,i, str_n);
         return 0;
      }
      i = separators.find(word);
      if (i != separators.size())
      {
         lexemes.emplace_back(separators.getRaw(i).type,i, str_n);
         return 0;
      }
   
      if (isIdentifier(word))
      {
         size_t i = identificators.find(word);
         if (i == identificators.size())
         {
            i = identificators.add(word, ExpandedToken::IDENTIFIER);
         }
         lexemes.emplace_back(identificators.getRaw(i).type,i, str_n);
         return 0;
      }
      if (isConstant(word))
      {
         size_t i = constants.find(word);
         if (i == constants.size())
         {
            i = constants.add(word, ExpandedToken::CONSTANT);
         }
         lexemes.emplace_back(constants.getRaw(i).type,i, str_n);
         return 0;
      }
      return 1;

   }
   int getPriority(const std::string &op)
   {
      if (op == "*" || op == "/")
         return 3;
      if (op == "+" || op == "-")
         return 2;
      if (op == "<")
         return 1;
      if (op == "==" || op == "!=")
         return 0;

      return -1;
   }
   void precompile(std::string &codepath,std::stringstream &stream)
   {
      std::ifstream code(codepath);
      bool comstate = false;
      size_t strn = 1;
      while (!code.eof())
      {
         /**/
         std::string str = "";
         std::getline(code, str);
         
         if (comstate)
         {

            auto commend = str.find("*/");
            if (commend != std::string::npos)
            {
               str.erase(str.begin(), str.begin() + commend + 2);
               comstate = false;
            }
            else
            {
               str.erase(str.begin(), str.end());
               stream << std::endl;
               strn++;
               continue;
            }
         }

         auto comm = str.find("/*");
         if (comm != std::string::npos)
         {

            auto commend = str.find("*/");
            if (commend != std::string::npos)
            {
               str.erase(str.begin() + comm, str.begin() + commend);
            }
            else
            {
               str.erase(str.begin() + comm, str.end());
               comstate = true;
            }
         }
         trim(str);
         
         for(auto &a : str)
         {
            if(a < 0)
            {
               str = "";
               makeError(strn, "forbidden symbol in string");
               break;
            }
         }

         auto cons = str.find("const");
         if (cons != std::string::npos)
         {
            auto sep = str.find(";");
            if (sep != std::string::npos)
            {
               str.erase(sep,1);
               std::stringstream buff(str);
               std::string strBuff = "";
               buff >> strBuff;
               if (strBuff == "const")
               {
                  buff >> str;
                  if (str == "int")
                  {
                     buff >> str;
                     std::string name = str;
                     if(isIdentifier(str))
                     {
                        if (constants.contains(str))
                        {
                           makeError(strn,str + " constant already defined");
                        }
                        else
                        {
                           buff >> str;
                           if (str == "=")
                           {
                              buff >> str;
                              if (isConstant(str))
                              {
                                 int *value = new int(std::stoi(str));
                                 constants.add(name, ExpandedToken::CONSTANT, new LexemeAttributes(value));
                              }
                              else
                              {
                                 makeError(strn, " expected number");
                              }
                           }
                           else
                           {
                              makeError(strn, " expected =");
                           }
                        }
                     }
                     else
                     {
                        makeError(strn, str + " user-defined literal not found or unappropriate");
                     }
                  }
                  else
                  {
                     makeError(strn, "unexpected const definition");
                  }
               }
               else
               {
                  makeError(strn, " unexpected const definition");
               }
            }
            else
            {
               makeError(strn, " lost ;");
            }
            str = "";
         }

         stream << str << std::endl;
         strn++;
      }
      if (comstate == true)
      {
         makeError(strn, "comment unclosed at end of file");
      }
   
   
   
   }
   void lexScan(std::istream& code)
   {
       size_t strn = 1;
       std::string str;

       while (std::getline(code, str))
       {
           auto start = str.begin();
           while (start != str.end())
           {
               if (*start == ' ')
               {
                   ++start;
                   continue;
               }

               auto separ = findSep(start, str.end(), stringSep);

               if (start != separ)
               {
                   std::string word(start, separ);
                   int err = addLexeme(word, strn);
                   if (err != 0)
                   {
                       makeError(strn, word + " user-defined literal not found or unappropriate");
                       break;
                   }
               }

               if (separ == str.end())
               {
                   break;
               }
               if (*separ != ' ')
               {
                   std::string sepWord(1, *separ);
                   int err = addLexeme(sepWord, strn);
                   if (err != 0)
                   {
                       makeError(strn, sepWord + " user-defined literal not found or unappropriate");
                       break;
                   }
               }

               start = separ + 1;
           }
           strn += 1;
       }
   }
   void scipStr(std::vector<LexemeI>::iterator &start)
   {
      auto buff = start;
      while (start->str_n == buff->str_n)
      {
         
         buff++;
         if (buff == lexemes.end())
         {
            break;
         }
      }
      start = buff;
   }

   bool checkStr(std::vector<LexemeI>::iterator &lex)
   {
      if ((lex + 1) != lexemes.end())
      {
         if (lex->str_n != (lex + 1)->str_n)
         {
            return false;
         }
         return true;
      }
      return false;
   }

   bool MainChain(std::vector<LexemeI>::iterator &start)
   {
      
      if (start->type != ExpandedToken::TYPE)
      {
         makeError(start->str_n,
            "Instead of " + getLexemeStr(*start) + ", expected type.");

         return false;
      }

      start++;

      if (getLexemeStr(*start) != "main")
      {
         makeError(start->str_n,
            "Instead of " + getLexemeStr(*start) + ", expected main.");

         return false;
      }

      start++;

      if (getLexemeStr(*start) != "(")
      {
         makeError(start->str_n,
            "Instead of " + getLexemeStr(*start) + ", expected (.");

         return false;
      }

      start++;

      if (getLexemeStr(*start) != ")")
      {
         makeError(start->str_n,
            "Instead of " + getLexemeStr(*start) + ", expected ).");

         return false;
      }

      start++;
      global = new Scope();
      curScope = global;
      return true;
   }
   BlockNode* BlockChain(std::vector<LexemeI>::iterator &start, BaseNode* parent)
   {
      BlockNode*  block= new BlockNode(parent);
      if (getLexemeStr(*start) == "{")
      {
         start++;
         auto lex = start;
         while (getLexemeStr(*lex) != "}")
         {            
            if(lex->type == ExpandedToken::TYPE)
            {
               if (checkStr(lex))
               {
                  lex++;
                  if (lex->type == ExpandedToken::IDENTIFIER)
                  {
                     block->body.push_back(DeclarationChain(lex, block));
                  }
                  else
                  {
                     makeExpectEr(lex, "identifier");
                     scipStr(lex);
                  }
               }
               else
               {
                  makeUnexEnExEr(lex);
                  scipStr(lex);
               }
            }
            else if(lex->type == ExpandedToken::IDENTIFIER)
            {
               std::string id = getLexemeStr(*lex);
               if (curScope->existInScope(id))
               {
                  block->body.push_back(AssigmentChain(lex, block));
               }
               else
               {
                  makeError(lex->str_n, id + ", identifier not defined");
                  scipStr(lex);
               }
            }
            else if(getLexemeStr(*lex) == "if")
            {
               block->body.push_back(IfChain(lex, block));
            }
            else if (getLexemeStr(*lex) == "return")
            {
               auto ret = new ReturnNode(block);
               if (checkStr(lex))
               {
                  lex++;
                  ret->retVal = ExpressionChain(lex, ret);
                  block->body.push_back(ret);
                  lex++;
               }
               else
               {
                  makeUnexEnExEr(lex);
                  scipStr(lex);
               }
            }
            else
            {
               makeError(lex->str_n,
                  getLexemeStr(*lex) + ", was unexpected");
               scipStr(lex);
            }

            start = lex;
         }
         auto buff = curScope;
         curScope = curScope->parent;
         delete buff;
      }
      else
      {
         makeExpectEr(start, "{");
         scipStr(start);
      }
      return block;
   
   }
   DeclarationNode* DeclarationChain(std::vector<LexemeI>::iterator &start, BaseNode *parent)
   {
      DeclarationNode *ret = new DeclarationNode(EtoT(start->type), start->i, parent);
      auto lexStr = getLexemeStr(*start);
      if (!(curScope->existInScope(lexStr)) && !(constants.contains(lexStr)))
      {
         curScope->vars[lexStr] = true;

         if (checkStr(start))
         {
            auto lex = start + 1;
            if (getLexemeStr(*lex) == "=")
            {
               lex++;
               ret->addInit(ExpressionChain(lex, parent));
               if (getLexemeStr(*lex) == ";")
               {
                  lex++;
               }
               else
               {
                  makeExpectEr(lex, ";");
                  scipStr(start);
               }
            }
            else
            {
               makeError(lex->str_n,
                  "Instead of " + getLexemeStr(*lex) + ", expected =");
               scipStr(start);
            }
         }
         else
         {
            makeError(start->str_n,
               "Unexpected end of expression");
            scipStr(start);
         }
      }
      else
      {
         makeAlExEr(start);
      }
      scipStr(start);
      return ret;
   }
   AssignNode* AssigmentChain(std::vector<LexemeI>::iterator &start, BaseNode *parent)
   {
      auto lex = start;
      auto ret = new AssignNode(parent);
      ret->lhs = new IdetifierNode(EtoT(lex->type), lex->i, ret);
      if (checkStr(lex))
      {
         lex++;
         if (getLexemeStr(*lex) == "=")
         {
            if (checkStr(lex))
            {
               lex++;
               ret->rhs = ExpressionChain(lex, ret);
               if (getLexemeStr(*lex) == ";")
               {
                  lex++;
               }
               else
               {
                  makeExpectEr(lex, ";");
               }
            }
            else
            {
               makeUnexEnExEr(lex);
            }
         }
         else
         {
            makeError(lex->str_n,
               "Instead of " + getLexemeStr(*lex) + ", expected =");
         }
      }
      else
      {
         makeUnexEnExEr(lex);
      }
      scipStr(start);
      return ret;
   }
   IfNode* IfChain(std::vector<LexemeI>::iterator &start, BaseNode *parent)
   {
      auto ret = new IfNode(parent);
      if(checkStr(start))
      {
         auto lex = start + 1;
         if(getLexemeStr(*lex) == "(")
         {
            lex++;
            ret->expression = ExpressionChain(lex, parent);
            if(getLexemeStr(*lex) == ")")
            {
               lex++;
               if(getLexemeStr(*lex) == "{")
               {
                  curScope = new Scope(curScope);
                  ret->block = BlockChain(lex, ret);
                  lex++;
                  ret->elseNode = ElseChain(lex, ret);
               }
               else
               {
                  makeExpectEr(lex, "{");
               }
            }
            else
            {
               makeExpectEr(lex, ")");
            }
         }
         else
         {
            makeExpectEr(lex, "(");
         }
         start = lex;
      }
      else
      {
         makeError(start->str_n,
            "Unexpected end of expression");
         scipStr(start);
      }

      return ret;
   }
   IfNode* ElseChain(std::vector<LexemeI>::iterator &start, BaseNode *parent)
   {
      if(getLexemeStr(*start) == "else")
      {
         IfNode *ret;
         auto lex = start;
         lex++;
         if(getLexemeStr(*lex) == "if")
         {
            ret = IfChain(lex, parent);
         }
         else
         {
            ret = new IfNode(parent);
            curScope = new Scope(curScope);
            ret->block = BlockChain(lex, parent);
            lex++;
         }
         start = lex;
         return ret;
         
      }
      else
      {
         return nullptr;
      }
      
   }

   bool isEndExpr(std::vector<LexemeI>::iterator it)
   {
      std::string str = getLexemeStr(*it);

      return
         str == ";" ||
         str == ")" ||
         str == "}" ||
         str == ",";
   }
   BaseNode *PrimaryChain(
      std::vector<LexemeI>::iterator &start,
      BaseNode *parent)
   {
      if (start->type == ExpandedToken::IDENTIFIER)
      {
         auto buff = getLexemeStr(*start);
         if (!curScope->existInScope(buff))
         {
            makeError(start->str_n, buff + ", identifier not defined");
         }
         auto *node =
            new IdetifierNode(
               EtoT(start->type),
               start->i,
               parent
            );
         start++;
         return node;
      }

      if (start->type == ExpandedToken::CONSTANT)
      {
         auto *node =
            new ConstNode(
               EtoT(start->type),
               start->i,
               parent
            );

         start++;

         return node;
      }

      if (getLexemeStr(*start) == "(")
      {
         start++;

         BaseNode *expr =
            ExpressionChain(start, parent);

         if (expr == nullptr)
         {
            return nullptr;
         }

         if (getLexemeStr(*start) != ")")
         {
            makeError(
               start->str_n,
               "Expected )"
            );

            return nullptr;
         }

         start++;

         return expr;
      }

      makeError(
         start->str_n,
         "Expected identifier, constant or ("
      );

      return nullptr;
   }
   BaseNode *ExpressionChain(
      std::vector<LexemeI>::iterator &start,
      BaseNode *parent,
      int minPriority = 0)
   {
      BaseNode *left =
         PrimaryChain(start, parent);

      if (left == nullptr)
         return nullptr;

      while (start->type == ExpandedToken::BINARY)
      {
         std::string opStr =
            getLexemeStr(*start);

         int priority =
            getPriority(opStr);

         if (priority < minPriority)
            break;

         auto *op =
            new OperationNode(
               EtoT(start->type),
               start->i,
               parent
            );

         start++;

         BaseNode *right =
            ExpressionChain(
               start,
               op,
               priority + 1
            );

         op->lhs = left;
         op->rhs = right;

         left = op;
      }

      return left;
   }



   void sinScan()
   {
      ProgrammNode *area =
         new ProgrammNode();

      auto lex = lexemes.begin();

      if (MainChain(lex))
      {
         BlockNode *block =
            BlockChain(lex, area);

         if (block != nullptr)
         {
            area->childs.push_back(block);
         }
      }
      PrintPostfix(area);
   }
   void PrintPostfix(BaseNode *node)
   {
      if (!node)
         return;

      switch (node->getType())
      {
         case NodeType::Identifier:
         {
            auto *leaf =
               static_cast<IdetifierNode *>(node);

            std::cout
               << getLexemeStr(leaf->table, leaf->id)
               << " ";

            return;
         }
         case NodeType::Const:
         {
            auto *leaf =
               static_cast<ConstNode *>(node);

            std::cout
               << getLexemeStr(leaf->table, leaf->id)
               << " ";

            return;
         }
         case NodeType::Operation:
         {
            auto *op =
               static_cast<OperationNode *>(node);

            PrintPostfix(op->lhs);
            PrintPostfix(op->rhs);

            std::cout
               << getLexemeStr(op->table, op->id)
               << " ";

            return;
         }
         case NodeType::Declaration:
         {
            auto *d =
               static_cast<DeclarationNode *>(node);

            std::cout
               << getLexemeStr(d->table, d->id)
               << " ";

            if (d->init)
            {
               PrintPostfix(d->init);
            }

            std::cout << "DECL ";

            return;
         }
         case NodeType::Assign:
         {
            auto *a =
               static_cast<AssignNode *>(node);

            PrintPostfix(a->lhs);
            PrintPostfix(a->rhs);

            std::cout << "= ";

            return;
         }
         case NodeType::Return:
         {
            auto *r =
               static_cast<ReturnNode *>(node);

            if (r->retVal)
            {
               PrintPostfix(r->retVal);
            }

            std::cout << "RETURN ";

            return;
         }
         case NodeType::Block:
         {
            auto *b =
               static_cast<BlockNode *>(node);

            for (auto *stmt : b->body)
            {
               PrintPostfix(stmt);
               std::cout << "\n";
            }
            return;
         }
         case NodeType::If:
         {
            auto *b =
               static_cast<IfNode *>(node);
            PrintPostfix(b->expression);
            std::cout << "IF\n    ";
            PrintPostfix(b->block);
            if(b->elseNode != nullptr)
            {
               PrintPostfix(b->elseNode);
            }
            return;
         }
         case NodeType::Programm:
         {
            auto *p =
               static_cast<ProgrammNode *>(node);

            for (auto *c : p->childs)
            {
               PrintPostfix(c);
            }

            return;
         }

         default:
            return;
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
      stringSep.insert('\0');
      std::string codePath(testFolder + "main.cpp");
      std::stringstream codestream;
      precompile(codePath, codestream);
      lexScan(codestream);
      std::ofstream out("errorsLex.txt");
      Error::printErrors(out);
      out.close();
      Error::errors.clear();
      sinScan();

      out.open("errorsSin.txt");
      Error::printErrors(out);
      out.close();
      out.open("lexemes.txt");
      for(auto &a : lexemes)
      {
         out << getLexemeStr(a) << "  ( " << expandedTokenToString(a.type) << ", " << a.i << " ) " << a.str_n << ";\n";
      }
      out.close();

	}
};