#pragma once

#include "alumi/lexer/token.h"
#include "alumi/syntax_tree/tree_nodes.h"

namespace alumi
{
	namespace syntax_tree
	{

		class Error
		{
		public:
			Error();
		private:
		};

		class ModuleRoot
		{
		public:
			ModuleRoot(const Nodes& children);

			ChildGroups<1> groups;
		};

		class Declaration
		{
		public:
		private:
		};

		class IntegerLiteral
		{
		public:
		private:
		};

		class Expression
		{
		public:
		private:
		};

		class FunctionDefinition
		{
		public:
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