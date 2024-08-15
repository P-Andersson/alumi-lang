#include "alumi/syntax_tree/node_utils.h"

#include <cassert>

namespace alumi
{
	namespace syntax_tree
	{
		size_t get_failure_token_index(const ::alumi::parser::Result& result)
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

		std::vector<UnicodeCodePoint> as_string(const Node& node, const SyntaxTree& tree)
		{
			const auto& tokens = tree.source().tokens();

			auto token_indices = tree.direct_tokens(&node);
			std::vector<UnicodeCodePoint> result;
			size_t prev_token_i = 0;

			size_t text_start = 0;
			size_t text_end = 0;

			auto append_f = [&](size_t start, size_t end)
			{
				end = std::min(end, tree.source().text().size());
				for (size_t i = start; i < end; ++i)
				{
					auto c = tree.source().text()[i];
					if (c == '\n')
					{
						result.push_back('\\');
						result.push_back('n');
					}
					result.push_back(c);
				}
			};

			for (auto token_i : token_indices)
			{
				if ((token_i != token_indices[0]) && (prev_token_i + 1 < token_i))
				{
					append_f(text_start, text_end);
					if (text_start < text_end)
					{
						result.push_back('.');
						result.push_back('.');
						result.push_back('.');
					}
					text_start = tokens[token_i].pos().char_index();
				}
				else if ((token_i == token_indices[0]))
				{
					text_start = tokens[token_i].pos().char_index();
				}
				text_end = tokens[token_i].pos().char_index() + tokens[token_i].size();
				prev_token_i = token_i;
			}
			append_f(text_start, text_end);
			return result;
		}

		std::vector<UnicodeCodePoint> as_string_deep(const Node& node, const SyntaxTree& tree)
		{
			auto [start, end] = node.spans_tokens();
			const auto& tokens = tree.source().tokens();

			size_t text_start = tokens[start].pos().char_index();
			size_t text_end = tokens[end - 1].pos().char_index() + tokens[end - 1].size() - 1;
		
			std::vector<UnicodeCodePoint> result;
			for (size_t i = text_start; i < text_end; ++i)
			{
				auto c = tree.source().text()[i];
				if (c == '\n')
				{
					result.push_back('\\');
					result.push_back('n');
				}
				result.push_back(c);
			}
			return result;
		}
	}
}