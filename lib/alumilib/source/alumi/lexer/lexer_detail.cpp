#include "alumi/lexer/lexer_detail.h"

#include <utf8cpp/utf8.h>

namespace alumi
{
   LexerResult::LexerResult(LexerResults type, size_t backtrack_cols)
      : type(type)
      , backtrack_cols(backtrack_cols)
   {

   }

   Text::Text(const std::string& str)
   {
      for (auto ite = str.begin(); ite != str.end(); )
      {
         UnicodeCodePoint cp = utf8::next(ite, str.end());
         m_text.push_back(cp);
      }
   }

   LexerResult Text::check(UnicodeCodePoint cp, size_t index)
   {
      if (m_text[index] == cp)
      {
         if (index + 1 == m_text.size())
         {
            return LexerResult(LexerResults::Completed, 0);
         }
         return LexerResult(LexerResults::Continue, 0);
      }
      return LexerResult(LexerResults::Failed, 0);
   }

   LexerResult Text::terminate(size_t index)
   {
      return LexerResult(LexerResults::Failed, 0);
   }

   AnyOf::AnyOf(const std::string& str)
   {
      for (auto ite = str.begin(); ite != str.end(); )
      {
         UnicodeCodePoint cp = utf8::next(ite, str.end());
         m_permitted.insert(cp);
      }
   }

   LexerResult AnyOf::check(UnicodeCodePoint cp, size_t index)
   {
      if (m_permitted.contains(cp))
      {
         return LexerResult(LexerResults::Completed, 0);
      }
      return LexerResult(LexerResults::Failed, 0);
   }

   LexerResult AnyOf::terminate(size_t index)
   {
      return LexerResult(LexerResults::Failed, 0);
   }

   NotAnyOf::NotAnyOf(const std::string& str)
   {
      for (auto ite = str.begin(); ite != str.end(); )
      {
         UnicodeCodePoint cp = utf8::next(ite, str.end());
         m_not_permitted.insert(cp);
      }
   }

   LexerResult NotAnyOf::check(UnicodeCodePoint cp, size_t index)
   {
      if (!m_not_permitted.contains(cp))
      {
         return LexerResult(LexerResults::Completed, 0);
      }
      return LexerResult(LexerResults::Failed, 0);
   }

   LexerResult NotAnyOf::terminate(size_t index)
   {
      return LexerResult(LexerResults::Failed, 0);
   }
}