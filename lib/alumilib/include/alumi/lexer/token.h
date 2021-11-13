#pragma once

#include "alumi/text_pos.h"

#include <vector>

namespace alumi
{

   enum class TokenType
   {
      Operator,
      Assignment,
      Symbol,
      Literal,
      ScopeBegin,
      SubscopeBegin,
      SubScopeEnd,
      Seperator,
      FuncDeclare,
      If,
      Else,
      For,
      While,
      Indent,
      EndOfFile
   };


   class Token
   {
   public:
      Token(TokenType type, TextPos pos, size_t size);

      TextPos pos() const;
      size_t size() const;
      TokenType type() const;

      bool operator==(const Token& r) const;
      bool operator!=(const Token& r) const;
   private:
      TextPos m_pos;
      size_t m_size;
      TokenType m_type;
   };

   using Tokens = std::vector<Token>;

   bool operator==(const Tokens& l, const Tokens& r);
   bool operator!=(const Tokens& l, const Tokens& r);
}