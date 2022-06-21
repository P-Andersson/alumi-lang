#include "alumi/syntax_tree/nodes.h"

#include "alumi/parser/parser_parts.h"

namespace alumi
{
	namespace syntax_tree
	{
		namespace
		{
			class OpRecursiveNodeCount
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
					return 0;
				}
			};

			class OpChildGroupCount
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

			class OpGetChildGroup
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
		}

		Node::Node(const Type& node, const parser::ParseResult& res)
		: Node(node, res.get_subparser().start_token_index(), res.get_subparser().current_token_index())
		{

		}
		Node::Node(const Type& node, size_t m_token_start, size_t m_token_end)
			: m_actual(node)
			, m_token_start(m_token_start)
			, m_token_end(m_token_end)
		{
		}

		std::tuple<size_t, size_t> Node::spans_tokens() const
		{
			return { m_token_start, m_token_end };
		}

		size_t Node::recursive_child_count() const
		{
			OpRecursiveNodeCount op;
			return visit(op);
		}

		size_t Node::child_group_count() const
		{
			OpChildGroupCount op;
			return visit(op);
		}

		ChildGroup Node::child_group(size_t group_index) const
		{
			OpGetChildGroup op(group_index);
			return visit(op);
		}
	}
}

