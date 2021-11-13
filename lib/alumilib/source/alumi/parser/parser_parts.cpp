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
               if (cur.type() == TokenType::Indent)
               {
                  if (m_indent_stack.size() > 0 && m_indent_stack.back() > cur.size())
                  {
                     m_indent_stack.pop_back();
                  }
                  if (m_indent_stack.size() == 0 || m_indent_stack.back() < cur.size())
                  {
                     m_indent_stack.push_back(cur.size());
                  }
               }
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

      void Subparser::use_state(const Subparser& parser)
      {
         m_indent_stack = parser.m_indent_stack;
         m_current = parser.m_current;
      }

      size_t Subparser::get_distance() const
      {
         return m_current - m_start;
      }

      std::optional<size_t> Subparser::get_indent() const
      {
         if (m_indent_stack.empty())
         {
            return std::nullopt;
         }
         return m_indent_stack.back();
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



         ParseResult Indented::parse(Subparser& parent)
         {
            auto indent = parent.get_indent();
            auto token = parent.advance();

            if (token.type() == TokenType::Indent)
            {
               if (indent == std::nullopt || parent.get_indent() > indent)
               {
                  return ParseResult(ParseResult::Type::Success, parent, {});
               }
            }
            return ParseResult(ParseResult::Type::Failure, parent, {});
         }


         ParseResult Dedented::parse(Subparser& parent)
         {
            auto indent = parent.get_indent();
            auto token = parent.advance();

            if (token.type() == TokenType::Indent)
            {
               if (indent != std::nullopt && parent.get_indent().value_or(0) < indent)
               {
                  return ParseResult(ParseResult::Type::Success, parent, {});
               }
            }
            return ParseResult(ParseResult::Type::Failure, parent, {});
         }

         ParseResult NoIndent::parse(Subparser& parent)
         {
            auto indent = parent.get_indent();
            auto token = parent.advance();

            if (token.type() == TokenType::Indent)
            {
               if (indent == parent.get_indent())
               {
                  return ParseResult(ParseResult::Type::Success, parent, {});
               }
            }
            return ParseResult(ParseResult::Type::Failure, parent, {});
         }

   }
}