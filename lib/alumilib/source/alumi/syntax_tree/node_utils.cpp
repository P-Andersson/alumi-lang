#include "alumi/syntax_tree/node_utils.h"

#include <cassert>

namespace alumi
{
	namespace syntax_tree
	{
		size_t get_failure_token_index(const ::alumi::parser::ParseResult& result)
		{
			assert(result.get_type() == parser::ParseResult::Type::Failure);

			size_t index_offset = result.get_consumed();
			for (const auto& node : result.get_nodes())
			{
				auto [start, end] = node.spans_tokens();
				if (index_offset >= start)
				{
					index_offset += end - start;
				}
			}
			return index_offset;
		}
	}
}