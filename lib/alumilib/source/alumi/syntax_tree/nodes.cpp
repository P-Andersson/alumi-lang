#include "alumi/syntax_tree/nodes.h"

#include "alumi/parser/parser_parts.h"

namespace alumi
{
	namespace syntax_tree
	{
		Error::Error()
		{

		}


		void ModuleRoot::add_node(const Node& node)
		{
			m_nodes.push_back(node);
		}

		Node::Node(const Type& node, const parser::ParseResult& res)
		: Node(node, res.get_subparser().start_token().pos(), res.get_subparser().current_token().pos())
		{

		}
		Node::Node(const Type& node, TextPos m_start, TextPos m_end)
			: m_actual(node)
			, m_start(m_start)
			, m_end(m_end)
		{
		}

		std::tuple<TextPos, TextPos> Node::spans() const
		{
			return { m_start, m_end };
		}
	}
}
