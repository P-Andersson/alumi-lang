module;

#include <vector>

export module alccemy.lexer.token;

import alccemy.lexer.concepts;
import alccemy.lexer.text;

export namespace alccemy
{
   template<TokenSet TokenSetT>
   class Token
   {
   public:
      using Type = TokenSetT;

      Token(Type type, TextPos pos, size_t size)
         : m_pos(pos)
         , m_size(size)
         , m_type(type)
      {

      }

      TextPos pos() const
      {
         return m_pos;
      }

      size_t size() const
      {
         return m_size;
      }

      Type type() const
      {
         return m_type;
      }

      bool operator==(const Token& r) const
      {
         return m_pos == r.m_pos && m_size == r.m_size && m_type == r.m_type;
      }

      bool operator!=(const Token& r) const
      {
         return !(*this == r);
      }

   private:
      TextPos m_pos;
      size_t m_size;
      Type m_type;
   };

   template<TokenSet TokenSetT>
   using Tokens = std::vector<Token<TokenSetT>>;

   template<TokenSet TokenSetT>
   bool operator==(const Tokens<TokenSetT>& l, const Tokens<TokenSetT>& r)
   {
      if (l.size() == r.size())
      {
         for (size_t i = 0; i < l.size(); ++i)
         {
            if (l[i] != r[i])
            {
               return false;
            }
         }
         return true;
      }
   }


   template<TokenSet TokenSetT>
   bool operator!=(const Tokens<TokenSetT>& l, const Tokens<TokenSetT>& r)
   {
      return !(l == r);
   }
}