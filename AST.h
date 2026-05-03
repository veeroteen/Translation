#pragma once
#include "Tables.h"
#include <map>
enum class NodeType
{
	Programm,

	Function,
	Block,

	Declaraation,
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
	DeclarationNode(TokenType table, size_t id, BaseNode *parent) : table(table), id(id), BaseNode(NodeType::Declaraation, parent) {};
	void addInit(BaseNode* node)
	{
		init = node;
	}
};
class AssignNode : public BaseNode
{
public:
	AssignNode(BaseNode *parent) : BaseNode(NodeType::Assign, parent) {};
	BaseNode *lhs = nullptr, *rhs = nullptr;
	void setLhs(BaseNode* node)
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

