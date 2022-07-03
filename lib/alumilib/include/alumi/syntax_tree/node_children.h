#pragma once

#include <array>

namespace alumi
{
	namespace syntax_tree
	{
		//!
		//! Reperesents a group of 0 or more child nodes, providing
		//! sufficient information to iterate them
		//! 
		//! skips is the groups start index offset, counting 0 as the same node as the parent node
		//! count is how many nodes it consists of 
		//! 		 
		class ChildGroup
		{
		public:
			ChildGroup(size_t skips, size_t count);

			size_t get_skips() const;
			size_t get_count() const;

		private:
			size_t m_skips;
			size_t m_count;
		};

		//!
		//! Represents a set of serval child groups, where each group has its own meaning and purpose
		//! 		 (ie, group 0 is function parameters, group 1 is return type
		//! 
		template<size_t Groups>
		class ChildGroups
		{
		public:
			ChildGroups(const std::array<ChildGroup, Groups>& groups)
				: m_groups(groups)
			{

			}

			size_t recursive_subnode_count() const
			{
				const ChildGroup& group = m_groups[Groups - 1];
				return group.get_skips() + group.get_count();
			}

			ChildGroup& get_group(size_t index)
			{
				return m_groups[index];
			}

			const ChildGroup& get_group(size_t index) const
			{
				return m_groups[index];
			}

			size_t size() const
			{
				return m_groups.size();
			}

		private:
			std::array<ChildGroup, Groups> m_groups;
		};
	}
}