#pragma once

#include <variant>
#include <vector>
#include <memory>

namespace alumi
{
	namespace syntax_tree
	{
		class Node;

		using Nodes = std::vector<Node>;
	
		// Note: Node references should be represent as offsets in the shared node array, relative to the parent

		class Error
		{
		public:
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
				Declaration, 
				IntegerLiteral,
				Expression, 
				FunctionDefinition,
				FunctionCall, 
				Brancher>;

			Node(const Type& node);

			template<typename T>
			bool is() const
			{
				return std::holds_alternative<T>(m_actual);
			}

		private:
			Type m_actual;
		};

	}

}