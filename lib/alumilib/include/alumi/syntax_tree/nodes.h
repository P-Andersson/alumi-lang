#pragma once

#include "alumi/lexer/token.h"
#include "alumi/syntax_tree/tree_nodes.h"
#include "alumi/syntax_tree/node_detail.h"

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
				Declaration, 
				IntegerLiteral,
				Expression, 
				FunctionDefinition,
				FunctionCall, 
				Brancher>;

			Node(const Type& node, const parser::ParseResult& res);
			Node(const Type& node, TextPos m_start, TextPos m_end);


			std::tuple<TextPos, TextPos> spans_text() const;

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

			TextPos m_start;
			TextPos m_end;
		};

	}

}