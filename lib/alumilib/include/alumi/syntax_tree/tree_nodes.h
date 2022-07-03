#pragma once

#include "alumi/syntax_tree.h"

#include <vector>
#include <array>
#include <concepts>

namespace alumi
{
	namespace syntax_tree
	{
		class SyntaxTreeWalker;

		template <typename FunctorT>
		concept NodeOp = requires (FunctorT fun, SyntaxTreeWalker & walker, Node& node) { fun(walker, node); };


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
		//! Walks a syntax tree, performing some operation on each node
		//! 
		class SyntaxTreeWalker
		{
		public:
			SyntaxTreeWalker(SyntaxTree& tree);


			template <NodeOp FuncT>
			void walk_from_root(FuncT& func)
			{
				NodeView cur_view(m_tree.nodes());
				for (auto node = cur_view.begin(); node != cur_view.end(); ++node)
				{
					SyntaxTreeWalker child(m_tree, node);
					func(child, *node);
				}
			}

			template <NodeOp FuncT>
			void walk(FuncT& func, const ChildGroup& group)
			{
				NodeView cur_view(m_tree.nodes(), m_current_root_node, group);
				auto begin = cur_view.begin();
				auto end = cur_view.end();
				for (auto node = begin; node != end; ++node)
				{
					SyntaxTreeWalker child(m_tree, node);
					func(child, *node);
				}
			}
		private:
			SyntaxTreeWalker(SyntaxTree& tree, NodeView::iterator root_node);
			SyntaxTree& m_tree;

			NodeView::iterator m_current_root_node;
		};

	}
}