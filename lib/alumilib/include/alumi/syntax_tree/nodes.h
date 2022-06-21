#pragma once

#include "alumi/lexer/token.h"
#include "alumi/syntax_tree/tree_nodes.h"
#include "alumi/syntax_tree/node_types.h"

#include <variant>

namespace alumi
{
	namespace parser
	{
		class ParseResult;
	}
	namespace syntax_tree
	{

		//!
		//! Reprents a single node in a node tree
		//! 
		//! Holds type specific data plus shared data
		//! 
		class Node
		{
		public:
			using Type = std::variant<
				Error, 
				ModuleRoot,
				CodeBlock,
				Assignment,
				IntegerLiteral,
				Expression, 
				Statement,
				FunctionDefinition,
				FunctionCall, 
				Brancher>;

			Node(const Type& node, const parser::ParseResult& res);
			Node(const Type& node, size_t m_token_start, size_t m_token_end);


			std::tuple<size_t, size_t> spans_tokens() const;

			size_t recursive_child_count() const;
			size_t child_group_count() const;
			ChildGroup child_group(size_t group_index) const;

			template<typename VisitorT>
			auto visit(VisitorT& visitor) const
			{
				return std::visit(visitor, m_actual);
			}

			template<typename VisitorT>
			auto visit(VisitorT& visitor)
			{
				return std::visit(visitor, m_actual);
			}

			template<typename VisitorT>
			auto visit(const VisitorT& visitor) const
			{
				return std::visit(visitor, m_actual);
			}

			template<typename VisitorT>
			auto visit(const VisitorT& visitor)
			{
				return std::visit(visitor, m_actual);
			}

	

			template<typename T>
			bool is() const
			{
				return std::holds_alternative<T>(m_actual);
			}

		private:
			Type m_actual;

			size_t m_token_start;
			size_t m_token_end;
		};

	}

}