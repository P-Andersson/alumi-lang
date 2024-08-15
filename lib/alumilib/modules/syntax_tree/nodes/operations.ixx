module;

#include "alumi/syntax_tree/node_children.h"
#include <variant>

export module alumi.syntax_tree.nodes.operations;


//!
//! Standard operations for a node that 
//! uses type-specific data
//! 

using namespace alumi::syntax_tree;

export class OpRecursiveNodeCount
{
public:
	template <typename T> requires requires (const T& t) { t.groups; }
	size_t operator()(const T& obj) const
	{
		return obj.groups.recursive_subnode_count();
	}

	template <typename T>
	size_t operator()(const T& obj) const
	{
		return 1;
	}
};

export class OpChildGroupCount
{
public:
	template <typename T> requires requires (const T& t) { t.groups; }
	size_t operator()(const T& obj) const
	{
		return obj.groups.size();
	}

	template <typename T>
	size_t operator()(const T& obj) const
	{
		return 0;
	}
};

export class OpGetChildGroup
{
public:
	OpGetChildGroup(size_t index)
		: m_index(index)
	{

	}

	template <typename T> requires requires (const T& t) { t.groups; }
	ChildGroup operator()(const T& obj) const
	{
		if (m_index >= obj.groups.size())
		{
			throw std::exception(); // TODO Better exception
		}
		return obj.groups.get_group(m_index);
	}

	template <typename T>
	ChildGroup operator()(const T& obj) const
	{
		throw std::exception(); // TODO Better exception
	}

private:
	size_t m_index;
};
