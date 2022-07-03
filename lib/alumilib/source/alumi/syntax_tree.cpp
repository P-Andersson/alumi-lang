#include "alumi/syntax_tree.h"

#include <cassert>

namespace alumi
{
   SyntaxTree::SyntaxTree(const parser::ParseResult& result, const LexedText* source)
      : m_result(result)
      , m_current_nodes(result.get_nodes())
      , m_source(source)
   {

   }

   const parser::ParseResult& SyntaxTree::parse_result() const
   {
      return m_result;
   }

   syntax_tree::Nodes& SyntaxTree::nodes()
   {
      return m_current_nodes;
   }

   const syntax_tree::Nodes& SyntaxTree::nodes() const
   {
      return m_current_nodes;
   }

   const LexedText& SyntaxTree::source() const
   {
      return *m_source;
   }

   std::vector<size_t> SyntaxTree::direct_tokens(const syntax_tree::Node* node) const
   {
      assert(node >= m_current_nodes.data() && node <= m_current_nodes.data() + m_current_nodes.size());

      std::vector<size_t> results;
      auto [token_start, token_end] = node->spans_tokens();
      std::vector<std::tuple<size_t, size_t>> holes;

      for (size_t group_index = 0; group_index < node->child_group_count(); ++group_index)
      {
         auto group = node->child_group(group_index);
         auto children_start = node + group.get_skips();
         auto children_end = node + group.get_skips() + group.get_count();

         size_t hole_start = std::get<0>((node + group.get_skips())->spans_tokens()); 

         size_t hole_end = hole_start;
         for (auto child = children_start; child < children_end; ++child)
         {
            hole_end = std::max(hole_end, std::get<1>(child->spans_tokens()));
         }
         holes.push_back({ hole_start, hole_end - 1 });
      }
      
      size_t hole_index = 0;
      size_t next_hole = token_end;
      if (holes.size() > 0)
      {
         next_hole = std::get<0>(holes[0]);
      }
      for (size_t token_candidate = token_start; token_candidate < token_end;  ++token_candidate)
      { 
         if (token_candidate == next_hole)
         {
            token_candidate = std::get<1>(holes[hole_index]);
            hole_index += 1;
            if (hole_index < holes.size())
            {
               next_hole = std::get<0>(holes[hole_index]);
            }
            else
            {
               next_hole = token_end;
            }
         }
         else
         {
            results.push_back(token_candidate);
         }
      }
      return results;
   }
}