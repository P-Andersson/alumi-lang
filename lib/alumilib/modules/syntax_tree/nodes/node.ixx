module;
#include "alumi/lexer/token.h"
#include "alumi/syntax_tree/node_children.h"

#include <variant>

export module alumi.syntax_tree.nodes.node;

import alumi.syntax_tree.nodes.types;

using namespace alumi::syntax_tree;

export
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
			FunctionDecleration,
			FunctionDefinition,
			FunctionCall,
			Brancher>;

		Node(const Type& node, size_t m_token_start, size_t m_token_end)
			: m_actual(node)
			, m_token_start(m_token_start)
			, m_token_end(m_token_end)
		{
		}


		std::tuple<size_t, size_t> spans_tokens() const
		{
			return { m_token_start, m_token_end };
		}

		//! 
		//! Returns the total number nodes this group takes up, including itself and any children
		//! 
		size_t recursive_child_count() const
		{
			OpRecursiveNodeCount op;
			return visit(op);
		}
		size_t child_group_count() const
		{
			OpChildGroupCount op;
			return visit(op);
		}
		ChildGroup child_group(size_t group_index) const
		{
			OpGetChildGroup op(group_index);
			return visit(op);
		}

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

		template<typename T>
		const T& as() const
		{
			assert(is<T>());
			return std::get<T>(m_actual);
		}


	private:
		Type m_actual;

		size_t m_token_start;
		size_t m_token_end;
	};

	using Nodes = std::vector<Node>;
}
