//AST.h
#pragma once
#include "Tables.h"
#include <map>
enum class NodeType
{
   Programm,

   Function,
   Block,

   Declaration,
   Assign,

   If,

   Operation,

   Identifier,
   Const,

   Return
};

class BaseNode
{
protected:
   NodeType type;
   BaseNode(NodeType type,BaseNode* parent) :type(type), parent(parent) {};
public:
   NodeType getType()
   {
      return type;
   }
   BaseNode *parent = nullptr;
   virtual ~BaseNode() = default;
};
class IdetifierNode : public BaseNode
{
public:
   TokenType table;
   size_t id;
   IdetifierNode(TokenType table, size_t id, BaseNode *parent) : table(table), id(id), BaseNode(NodeType::Identifier,parent) {};
};
class ConstNode : public BaseNode
{
public:
   TokenType table;
   size_t id;
   ConstNode(TokenType table, size_t id, BaseNode *parent) : table(table), id(id), BaseNode(NodeType::Const, parent) {};
};
class ProgrammNode : public BaseNode
{
public:
   ProgrammNode() : BaseNode(NodeType::Programm, nullptr) {};
   std::vector<BaseNode *> childs;
};
class BlockNode : public BaseNode
{
public:
   std::vector<BaseNode *> body;
   BlockNode(BaseNode *parent) : BaseNode(NodeType::Block, parent) {};
   void addNode(BaseNode* node)
   {
      body.push_back(node);
   }
};
class DeclarationNode : public BaseNode
{
public:
   TokenType table;
   size_t id;
   BaseNode *init = nullptr;
   DeclarationNode(TokenType table, size_t id, BaseNode *parent) : table(table), id(id), BaseNode(NodeType::Declaration, parent) {};
   void addInit(BaseNode* node)
   {
      init = node;
   }
};
class AssignNode : public BaseNode
{
public:
   AssignNode(BaseNode *parent) : BaseNode(NodeType::Assign, parent) {};
   IdetifierNode *lhs = nullptr;
   BaseNode *rhs = nullptr;
   void setLhs(IdetifierNode* node)
   {
      lhs = node;
   }
   void setRhs(BaseNode *node)
   {
      rhs = node;
   }
};
class IfNode : public BaseNode
{
public:
   IfNode(BaseNode *parent) : BaseNode(NodeType::If, parent) {};
   BaseNode *expression = nullptr;
   BlockNode *block = nullptr;
   BaseNode *elseNode = nullptr;
};
class OperationNode : public BaseNode
{
public:
   OperationNode(TokenType table, size_t id, BaseNode *parent) : table(table), id(id), BaseNode(NodeType::Operation, parent) {};
   TokenType table;
   size_t id;
   BaseNode *lhs = nullptr;
   BaseNode *rhs = nullptr;
   void setOperands(BaseNode* left, BaseNode* right)
   {
      lhs = left;
      rhs = right;
   }

};
class ReturnNode : public BaseNode
{
public:
   ReturnNode(BaseNode *parent) : BaseNode(NodeType::Return, parent) {};
   BaseNode *retVal;
};

enum class Levels
{
   PROGRAMM,
   BLOCK,
   EXPRESSION,
   DECLARATION,
   ASSIGMENT,
   OPEARATOR,
   SEP,
   IFSEQ,
   ELSE
};

class Scope
{
public:

   Scope *parent = nullptr;

   std::unordered_map<std::string, bool> vars;

   Scope() = default;
   Scope(Scope *parent) : parent(parent) {};
   bool existInScope(const std::string &lex)
   {
      auto scope = this;
      do
      {
         if(scope->vars.contains(lex))
         {
            return true;
         }
         scope = scope->parent;
      } while (scope != nullptr);
      return false;
   }

};

