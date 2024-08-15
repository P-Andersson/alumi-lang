module;

#include "alumi/lexer/token.h"

#include <vector>
#include <optional>

export module alumi.parser.subparser;

using namespace alumi;

//!
//! Represents a parsing view of a single token stream
//! 
//! 
export class Subparser
{
public:
   Subparser(const std::vector<Token>& tokens)
      : m_token_source(&tokens)
      , m_start(0)
      , m_current(0)
      , m_panic_token_index()
   {
   }

   Subparser create_child() const
   {
      Subparser child = *this;
      child.m_start = m_current;
      return child;
   }

   const std::vector<Token>& tokens() const
   {
      return *m_token_source;
   }

   //!
   //! Gets next token and advances internal token index by 1
   //! If past then end of the token stream, instead returns the last token
   //! 
   Token advance()
   {
      do
      {
         if (m_current >= m_token_source->size())
         {
            return m_token_source->back();
         }
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

   //!
   //! Returns the next token
   //! If past the end of the token stream, instead returns the last token
   //! 
   Token peek() const
   {
      if (m_current >= m_token_source->size())
      {
         return m_token_source->back();
      }

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

   Token start_token() const
   {
      return (*m_token_source)[m_start];
   }

   Token current_token() const
   {
      if (m_current < m_token_source->size())
      {
         return (*m_token_source)[m_current];
      }
      return (*m_token_source)[m_token_source->size() - 1];

   }

   size_t start_token_index() const
   {
      return m_start;
   }

   size_t current_token_index() const
   {
      return m_current;
   }

   //!
   //! Points the token index to this parsers start posiiton, not to zero
   //! 
   void restart_parsing()
   {
      m_current = m_start;
   }

   //!
   //! Copies the state of a child-parser that should not be scope-limited to a sub-expression,
   //! such current token and the indention data, so that it can resume parsing from where the
   //! child left off
   //! 
   void take_over_from(const Subparser& parser)
   {
      m_indent_stack = parser.m_indent_stack;
      m_current = parser.m_current;
      m_panic_token_index = parser.m_panic_token_index;
   }

   size_t get_distance() const
   {
      return m_current - m_start;
   }

   std::optional<size_t> get_indent() const
   {
      if (m_indent_stack.empty())
      {
         return std::nullopt;
      }
      return m_indent_stack.back();
   }

   // TODO Consider how to handle panicing, note that RECOVERED ERRORS paniced because a subparser paniced and not themselves

   //!
   //! Marks this parser, along with further children, 
   //! to swallow and ignore further tokens of this type
   //! 
   void add_swallowed_token(TokenType type)
   {
      m_swallowed.push_back(type);
   }

   //!
   //! Tells this subparser to enter a panic (error) state, signifying that some parser issue
   //! has been encountered.
   //! @param trigger_token_index   the index of token that caused the problem
   //!  
   void do_panic(size_t triggering_token_index)
   {
      m_panic_token_index = triggering_token_index;
   }

   //!
   //! Clears panic state, marking that the error could be recovered from
   //! 
   void clear_panic()
   {
      m_panic_token_index = std::nullopt;
   }


   //!
   //! True if this is in a state of panic or not
   //! 
   bool is_panicing() const
   {
      return m_panic_token_index != std::nullopt;
   }

   //!
   //! Gets the idnex of the token that caused the panic, or nullopt if not
   //! 
   std::optional<size_t> get_panic_token_index() const
   {
      return m_panic_token_index;
   }

private:
   const std::vector<Token>* m_token_source;
   std::vector<size_t> m_indent_stack;

   size_t m_start;
   size_t m_current;
   std::vector<TokenType> m_swallowed;

   std::optional<size_t> m_panic_token_index;

};