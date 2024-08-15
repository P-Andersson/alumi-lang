module;
#include "alumi/lexer/token.h"
#include "alumi/syntax_tree/node_children.h"

#include <variant>
#include <optional>

export module alumi.syntax_tree.nodes.types;

import alumi.parser.error_codes;

using namespace alumi;
using namespace alumi::syntax_tree;

class Node;

export
{
	using Nodes = std::vector<Node>;

	class Error
	{
	public:
		Error(ErrorCode ec, size_t token_index, const Nodes& children);


		ChildGroups<1> groups;
		ErrorCode error_code;
		size_t token_index;
	private:

	};
	class ModuleRoot
	{
	public:
		ModuleRoot(const Nodes& children);

		ChildGroups<1> groups;
	};

	class CodeBlock
	{
	public:
		CodeBlock(const Nodes& children);

		ChildGroups<1> groups;
	};


	class Assignment
	{
	public:
		Assignment(const Node& value);

		ChildGroups<1> groups;
	};

	class IntegerLiteral
	{
	public:
	private:
	};

	class Statement
	{
	public:
		Statement(const Nodes& children);

		ChildGroups<1> groups;
	private:
	};

	class Expression
	{
	public:
	private:
	};

	class FunctionDecleration
	{
	public:
		FunctionDecleration(const Node& params, const std::optional<Node>& return_type);

		ChildGroups<2> groups;
	private:
	};

	class FunctionDefinition
	{
	public:
		FunctionDefinition(const Node& decleration, const Node& code);

		ChildGroups<2> groups;
	private:
	};

	class FunctionCall
	{
	public:
	private:
	};

	class Brancher
	{
	public:
	private:
	};

}
