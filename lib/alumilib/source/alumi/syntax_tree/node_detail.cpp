#include "alumi/syntax_tree/node_detail.h"

#include "alumi/syntax_tree/nodes.h"

namespace alumi
{
	namespace syntax_tree
	{
		Error::Error()
		{

		}

		ModuleRoot::ModuleRoot(const Nodes& children)
			: groups({ ChildGroup(0, children.size()) })
		{

		}
	}
}
