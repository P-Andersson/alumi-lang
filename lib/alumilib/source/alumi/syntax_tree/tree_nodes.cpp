#include "alumi/syntax_tree/tree_nodes.h"

#include "alumi/syntax_tree/nodes.h"


namespace alumi
{
	namespace syntax_tree
	{
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
			return (*this) += 1;
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
				m_inner += (**this).recursive_child_count();
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
			, m_first(parent.m_inner)
			, m_group_viewed(children)
		{

		}

		
		NodeView NodeView::child_view(NodeView::iterator parent, const ChildGroup& children)
		{
			NodeView child(*this);
			child.m_first = parent.m_inner;
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

		SyntaxTreeWalker::SyntaxTreeWalker(SyntaxTree& tree)
			: m_tree(tree)
			, m_current_root_node(NodeView(m_tree.nodes()).begin())
		{

		}

		SyntaxTreeWalker::SyntaxTreeWalker(SyntaxTree& tree, NodeView::iterator root_node)
			: m_tree(tree)
			, m_current_root_node(root_node)
		{

		}
	

	}
}