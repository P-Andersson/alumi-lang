#pragma once

#include <vector>

namespace alumi
{
	namespace syntax_tree
	{
		class Node;
		using Nodes = std::vector<Node>;

		class Symbol
		{

		};

		class ModuleTree
		{
		public:
			ModuleTree( const Nodes& nodes);

			Nodes& nodes();

		private:
			Nodes m_nodes;
		};
	}
}