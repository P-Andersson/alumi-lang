#include "alumi/syntax_tree/nodes.h"

namespace alumi
{
	namespace syntax_tree
	{
		void ModuleRoot::add_node(const Node& node)
		{
			m_nodes.push_back(node);
		}

		Node::Node(const Type& node)
			: m_actual(node)
		{
		}
	}
}
