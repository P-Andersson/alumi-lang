#include "alumi/syntax_tree/modue_tree.h"

#include "alumi/syntax_tree/nodes.h"

namespace alumi
{
	namespace syntax_tree
	{
		ModuleTree::ModuleTree(const Nodes& nodes)
			: m_nodes(nodes)
		{

		}

		Nodes& ModuleTree::nodes()
		{
			return m_nodes;
		}
	}
}