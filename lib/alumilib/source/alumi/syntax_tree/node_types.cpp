#include "alumi/syntax_tree/node_types.h"

#include "alumi/syntax_tree/nodes.h"

namespace alumi
{
	namespace syntax_tree
	{
		Error::Error(const Nodes& children)
			: groups({ ChildGroup(0, children.size()) })
		{

		}

		ModuleRoot::ModuleRoot(const Nodes& children)
			: groups({ ChildGroup(0, children.size()) })
		{

		}

		CodeBlock::CodeBlock(const Nodes& children)
			: groups({ ChildGroup(0, children.size()) })
		{

		}
	}
}
