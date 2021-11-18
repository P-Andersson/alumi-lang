#pragma once

#include "alumi/lexer/token.h"

#include <variant>
#include <vector>
#include <memory>

namespace alumi
{
	namespace parser
	{
		class ParseResult;
	}

	namespace syntax_tree
	{
		class Node;

		using Nodes = std::vector<Node>;
	
		// Note: Node references should be represent as offsets in the shared node array, relative to the parent

		class Error
		{
		public:
			Error();
		private:
		};

		class ModuleRoot
		{
		public:
			void add_node(const Node& node);

		private:
			Nodes m_nodes;
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


		class Node
		{
		public:
			using Type = std::variant<
				Error, 
				ModuleRoot,
				Declaration, 
				IntegerLiteral,
				Expression, 
				FunctionDefinition,
				FunctionCall, 
				Brancher>;

			Node(const Type& node, const parser::ParseResult& res);
			Node(const Type& node, TextPos m_start, TextPos m_end);


			std::tuple<TextPos, TextPos> spans() const;

			template<typename T>
			bool is() const
			{
				return std::holds_alternative<T>(m_actual);
			}

		private:
			Type m_actual;

			TextPos m_start;
			TextPos m_end;
		};

	}

}