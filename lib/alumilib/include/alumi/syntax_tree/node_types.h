#pragma once

#include "alumi/lexer/token.h"
#include "alumi/parser/parser_error_codes.h"
#include "alumi/syntax_tree/node_children.h"

#include <optional>

namespace alumi
{
	namespace syntax_tree
	{
		class Node;
		using Nodes = std::vector<Node>;

		class Error
		{
		public:
			Error(parser::ErrorCode ec, size_t token_index, const Nodes& children);

			ChildGroups<1> groups;
			parser::ErrorCode error_code;
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

}