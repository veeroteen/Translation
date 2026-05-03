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

      return true;
   }

   BaseNode *PrimaryChain(
      std::vector<LexemeI>::iterator &start,
      BaseNode *parent)
   {
      if (start->type == ExpandedToken::IDENTIFIER)
      {
         BaseNode *node =
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
         BaseNode *node =
            new ConstNode(
               EtoT(start->type),
               start->i,
               parent
            );

         start++;

         return node;
      }

      makeError(
         start->str_n,
         "Expected identifier or constant."
      );

      return nullptr;
   }

   BaseNode *ExpressionChain(
      std::vector<LexemeI>::iterator &start,
      BaseNode *parent)
   {
      BaseNode *left = PrimaryChain(start, parent);

      if (left == nullptr)
      {
         return nullptr;
      }

      while (start->type == ExpandedToken::BINARY)
      {
         OperationNode *op =
            new OperationNode(
               EtoT(start->type),
               start->i,
               parent
            );

         start++;

         BaseNode *right =
            PrimaryChain(start, op);

         if (right == nullptr)
         {
            delete op;
            return left;
         }

         op->setOperands(left, right);

         left = op;
      }

      return left;
   }

   void DeclarationChain(
      DeclarationNode *node,
      std::vector<LexemeI>::iterator &start)
   {
      if (getLexemeStr(*start) == ";")
      {
         start++;
         return;
      }

      if (getLexemeStr(*start) == "=")
      {
         start++;

         BaseNode *expr =
            ExpressionChain(start, node);

         if (expr == nullptr)
         {
            makeError(
               start->str_n,
               "Expected expression."
            );

            return;
         }

         node->addInit(expr);

         if (getLexemeStr(*start) != ";")
         {
            makeError(
               start->str_n,
               "Expected ;"
            );

            return;
         }

         start++;

         return;
      }

      makeError(
         start->str_n,
         "Expected ; or ="
      );
   }

   void AssignChain(
      AssignNode *node,
      std::vector<LexemeI>::iterator &start)
   {
      if (start->type != ExpandedToken::IDENTIFIER)
      {
         makeError(
            start->str_n,
            "Expected identifier."
         );

         return;
      }

      node->setLhs(
         new IdetifierNode(
            EtoT(start->type),
            start->i,
            node
         )
      );

      start++;

      if (getLexemeStr(*start) != "=")
      {
         makeError(
            start->str_n,
            "Expected ="
         );

         return;
      }

      start++;

      BaseNode *expr =
         ExpressionChain(start, node);

      if (expr == nullptr)
      {
         makeError(
            start->str_n,
            "Expected expression."
         );

         return;
      }

      node->setRhs(expr);

      if (getLexemeStr(*start) != ";")
      {
         makeError(
            start->str_n,
            "Expected ;"
         );

         return;
      }

      start++;
   }


   void IfChain(
      IfNode *node,
      std::vector<LexemeI>::iterator &start)
   {
      if (getLexemeStr(*start) != "if")
      {
         makeError(
            start->str_n,
            "Expected if."
         );

         return;
      }

      start++;

      if (getLexemeStr(*start) != "(")
      {
         makeError(
            start->str_n,
            "Expected (."
         );

         return;
      }

      start++;

      BaseNode *expr =
         ExpressionChain(start, node);

      if (expr == nullptr)
      {
         makeError(
            start->str_n,
            "Expected expression."
         );

         return;
      }

      node->expression = expr;

      if (getLexemeStr(*start) != ")")
      {
         makeError(
            start->str_n,
            "Expected )."
         );

         return;
      }

      start++;

      node->block =
         BlockChain(start, node);

      if (getLexemeStr(*start) == "else")
      {
         start++;

         if (getLexemeStr(*start) == "if")
         {
            IfNode *elseIf =
               new IfNode(node);

            IfChain(elseIf, start);

            node->elseNode = elseIf;
         }
         else
         {
            node->elseNode =
               BlockChain(start, node);
         }
      }
   }

   BlockNode *BlockChain(
      std::vector<LexemeI>::iterator &start,
      BaseNode *parent)
   {
      if (getLexemeStr(*start) != "{")
      {
         makeError(
            start->str_n,
            "Instead of " + getLexemeStr(*start) + ", expected {."
         );

         return nullptr;
      }

      BlockNode *block =
         new BlockNode(parent);

      start++;

      while (getLexemeStr(*start) != "}")
      {
         if (start->type == ExpandedToken::TYPE)
         {
            start++;

            if (start->type != ExpandedToken::IDENTIFIER)
            {
               makeError(
                  start->str_n,
                  "Expected identifier."
               );

               return block;
            }

            DeclarationNode *decl =
               new DeclarationNode(
                  EtoT(start->type),
                  start->i,
                  block
               );

            block->addNode(decl);

            start++;

            DeclarationChain(decl, start);
         }
         else if (start->type == ExpandedToken::IDENTIFIER)
         {
            AssignNode *assign =
               new AssignNode(block);

            block->addNode(assign);

            AssignChain(assign, start);
         }
         else if (getLexemeStr(*start) == "if")
         {
            IfNode *ifnode =
               new IfNode(block);

            block->addNode(ifnode);

            IfChain(ifnode, start);
         }
         else if (getLexemeStr(*start) == "return")
         {
            start++;

            ReturnNode *retnode =
               new ReturnNode(block);

            block->addNode(retnode);

            if (getLexemeStr(*start) != ";")
            {
               retnode->retVal =
                  ExpressionChain(start, retnode);
            }

            if (getLexemeStr(*start) != ";")
            {
               makeError(start->str_n, "Expected ; after return");
               return nullptr;
            }

            start++;
         }
         else
         {
            makeError(
               start->str_n,
               "Unexpected token: " + getLexemeStr(*start)
            );

            start++;
         }
      }

      start++;

      return block;
   }
   void makeError(size_t strn, std::string str)
   {
      std::string error = "line: " + std::to_string(strn) + ", " + str;
      Error::errors.push_back(error);
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
      if (isInteger(word))
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
   void lexScan(std::string &codePath)
   {
      std::ifstream code(codePath);
      size_t strn = 1;
      bool comstate = false;
      while (!code.eof())
      {
         /**/
         std::string str = "";
         std::getline(code, str);
         if(comstate)
         {
            
            auto commend = str.find("*/");
            if (commend != std::string::npos)
            {
               str.erase(str.begin(), str.begin()+commend+2);
               comstate = false;
            }
            else
            {
               str.erase(str.begin(), str.end());
            
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
         auto start = str.begin();



         while (start != str.end())
         {
            auto separ = findSep(start, str.end(), stringSep);
            std::string word = std::string(start, separ);
            int err = addLexeme(word,strn);
            if (err != 0 && (separ == str.end() ? true : (*separ == ' ')))
            {
               makeError(strn, word + " user-defined literal not found or unappropriate");
            }
            if(separ == str.end())
            {
               break;
            }
            if(*separ != ' ')
            {
               word = std::string(separ, separ+1);
               err = addLexeme(word, strn);
               if (err != 0)
               {
                  makeError(strn, word + "  user-defined literal not found or unappropriate");

               }
            }
            start = separ+1;
         }
         strn += 1;
      }
      if(comstate == true)
      {
         makeError(strn,"comment unclosed at end of file");
      }
   
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
      if (!node) return;

      switch (node->getType())
      {
         case NodeType::Identifier:
         {
            auto *leaf = static_cast<IdetifierNode *>(node);
            std::cout << getLexemeStr(leaf->table, leaf->id) << " ";
            return;
         }
         case NodeType::Const:
         {
            auto *leaf = static_cast<ConstNode *>(node);
            std::cout << getLexemeStr(leaf->table, leaf->id) << " ";
            return;
         }

         case NodeType::Operation:
         {
            auto *op = static_cast<OperationNode *>(node);

            PrintPostfix(op->lhs);
            PrintPostfix(op->rhs);

            std::cout << getLexemeStr(op->table, op->id) << " ";
            return;
         }

         case NodeType::Assign:
         {
            auto *a = static_cast<AssignNode *>(node);

            PrintPostfix(a->lhs);
            PrintPostfix(a->rhs);

            std::cout << "= ";
            return;
         }

         case NodeType::Return:
         {
            auto *r = static_cast<ReturnNode *>(node);

            if (r->retVal)
               PrintPostfix(r->retVal);

            std::cout << "return ";
            return;
         }

         case NodeType::Block:
         {
            auto *b = static_cast<BlockNode *>(node);

            for (auto *stmt : b->body)
               PrintPostfix(stmt);

            return;
         }

         case NodeType::Programm:
         {
            auto *p = static_cast<ProgrammNode *>(node);

            for (auto *c : p->childs)
               PrintPostfix(c);

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
      lexScan(codePath);
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