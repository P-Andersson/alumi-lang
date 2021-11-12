#include "alumi/parser/parser_parts.h"

namespace alumi
{
   namespace parser
   {

      Subparser::Subparser(std::vector<Token>& tokens)
         : m_token_source(&tokens)
         , m_start(0)
         , m_current(0)
      {

      }

      Subparser Subparser::create_child() const
      {
         Subparser child = *this;
         child.m_start = m_current;
         return child;
      }


      Token Subparser::advance()
      {
         do
         {
            auto& cur = (*m_token_source)[m_current++];
            if (std::find(m_swallowed.begin(), m_swallowed.end(), cur.type()) == m_swallowed.end())
            {
               return cur;
            }
         } while (true);

         throw std::exception(); // TOOD better error reporting, as this must mean ignoring EOF?
      }

      Token Subparser::peek()
      {
         size_t offset = 0;
         do
         {
            auto& cur = (*m_token_source)[m_current + offset];
            offset += 1;
            if (std::find(m_swallowed.begin(), m_swallowed.end(), cur.type()) == m_swallowed.end())
            {
               return cur;
            }
         } while (true);

         throw std::exception(); // TOOD better error reporting, as this must mean ignoring EOF?
      }

      void Subparser::skip_forward(size_t steps)
      {
         m_current += steps;
      }

      size_t Subparser::get_distance() const
      {
         return m_current - m_start;
      }

      void Subparser::add_swallowed_token(TokenType type)
      {
         m_swallowed.push_back(type);
      }


      ParseResult::ParseResult(Type type,
         const Subparser& parser,
         const syntax_tree::Nodes& child_nodes)
         : m_type(type)
         , m_parser_used(parser)
         , m_nodes(child_nodes)
      {
      }

      ParseResult::Type ParseResult::get_type() const
      {
         return m_type;
      }

      const Subparser& ParseResult::get_subparser() const
      {
         return m_parser_used;
      }

      size_t ParseResult::get_consumed() const
      {
         return m_parser_used.get_distance();
      }

      const syntax_tree::Nodes& ParseResult::get_nodes() const
      {
         return m_nodes;
      }
   }
}