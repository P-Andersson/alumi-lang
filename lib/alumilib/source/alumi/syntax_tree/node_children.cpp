#include "alumi/syntax_tree/node_children.h"

namespace alumi
{
	namespace syntax_tree
	{
		ChildGroup::ChildGroup(size_t skips, size_t count)
			: m_skips(skips)
			, m_count(count)
		{

		}

		size_t ChildGroup::get_skips() const
		{
			return m_skips;
		}
		size_t ChildGroup::get_count() const
		{
			return m_count;
		}

	}
}