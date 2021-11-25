#pragma once

#include "alumi/syntax_tree/modue_tree.h"

#include <vector>
#include <array>
#include <concepts>

namespace alumi
{
	namespace syntax_tree
	{
		class ModuleTree;
		class ModuleTreeWalker;

		template <typename FunctorT>
		concept ModuleNodeOp = requires (FunctorT fun, ModuleTreeWalker& walker, Node& node) { fun(walker, node); };


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


		//!
		//! View of a group of nodes on the same level and with the same parent, for easy iteration
		//! 
		class NodeView
		{
		public:
			//!
			//! iteartor that skips child nodes, only passing through a nodes siblings
			//! 
			class iterator
			{
			public:
				iterator();

				Node& operator*();
				Node* operator->();
				iterator operator++();
				iterator operator+(size_t offset) const;
				iterator& operator+=(size_t offset);

				std::strong_ordering operator<=>(const iterator& other) const = default;

			private:
				using iterator_t = Nodes::iterator;

				iterator(iterator_t inner);


				iterator_t m_inner;
				friend class NodeView;
			};

			NodeView(Nodes& source);
			NodeView(Nodes& source, iterator parent, const ChildGroup& children);

			NodeView child_view(iterator parent, const ChildGroup& children);

			iterator begin();
			iterator end();

			size_t size() const;

		private:
			Nodes* m_nodes;
			Nodes::iterator m_first;

			ChildGroup m_group_viewed;
		};

		//!
		//! Walks a module tree, performing some operation on each node
		//! 
		class ModuleTreeWalker
		{
		public:
			ModuleTreeWalker(ModuleTree& node_tree);


			template <ModuleNodeOp FuncT>
			void walk_from_root(FuncT& func)
			{
				NodeView cur_view(m_node_tree.nodes());
				for (auto node = cur_view.begin(); node != cur_view.end(); ++node)
				{
					ModuleTreeWalker child(m_node_tree, node);
					func(child, *node);
				}
			}

			template <ModuleNodeOp FuncT>
			void walk(FuncT& func, const ChildGroup& group)
			{
				NodeView cur_view(m_node_tree.nodes(), m_current_root_node, group);
				for (auto node = cur_view.begin(); node != cur_view.end(); ++node)
				{
					ModuleTreeWalker child(m_node_tree, node);
					func(child, *node);
				}
			}
		private:
			ModuleTreeWalker(ModuleTree& node_tree, NodeView::iterator root_node);
			ModuleTree& m_node_tree;

			NodeView::iterator m_current_root_node;
		};

	}
}