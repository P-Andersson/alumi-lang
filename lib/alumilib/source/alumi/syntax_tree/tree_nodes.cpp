#include "alumi/syntax_tree/tree_nodes.h"

#include "alumi/syntax_tree/modue_tree.h"
#include "alumi/syntax_tree/nodes.h"


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


		NodeView::iterator::iterator()
		{

		}

		Node& NodeView::iterator::operator*()
		{
			return *m_inner;
		}

		Node* NodeView::iterator::operator->()
		{
			return &*m_inner;
		}

		NodeView::iterator NodeView::iterator::operator++()
		{
			return *this += 1;
		}
		NodeView::iterator NodeView::iterator::operator+(size_t offset) const
		{
			iterator new_view(*this);
			new_view += offset;
			return new_view;
		}
		NodeView::iterator& NodeView::iterator::operator+=(size_t offset)
		{
			for (size_t steps = 0; steps < offset; ++steps)
			{
				m_inner += (**this).recursive_child_count() + 1;
			}
			return *this;

		}

		NodeView::iterator::iterator(iterator_t inner)
			: m_inner(inner)
		{

		}

		NodeView::NodeView(Nodes& source)
			: m_nodes(&source)
			, m_first(source.begin())
			, m_group_viewed(0, source.size())
		{

		}

		NodeView::NodeView(Nodes& source, iterator parent, const ChildGroup& children)
			: m_nodes(&source)
			, m_first(parent.m_inner + 1)
			, m_group_viewed(children)
		{

		}

		
		NodeView NodeView::child_view(NodeView::iterator parent, const ChildGroup& children)
		{
			NodeView child(*this);
			child.m_first = parent.m_inner + 1;
			child.m_group_viewed = children;
			return child;
		}

		NodeView::iterator NodeView::begin()
		{
			return iterator(m_first + m_group_viewed.get_skips());
		}
		NodeView::iterator NodeView::end()
		{
			return iterator(m_first + m_group_viewed.get_skips() + m_group_viewed.get_count());
		}

		size_t NodeView::size() const
		{
			return m_group_viewed.get_count();
		}

		ModuleTreeWalker::ModuleTreeWalker(ModuleTree& node_tree)
			: m_node_tree(node_tree)
			, m_current_root_node(NodeView(m_node_tree.nodes()).begin())
		{

		}

		ModuleTreeWalker::ModuleTreeWalker(ModuleTree& node_tree, NodeView::iterator root_node)
			: m_node_tree(node_tree)
			, m_current_root_node(root_node)
		{

		}
	

	}
}