#pragma once

#include "alumi/text_pos.h"

#include <vector>

namespace alumi
{

   enum class TokenType
   {
      Operator         = 0,
      Assignment       = 1,
      ReturnOp         = 2,
      Noop             = 3,
      Symbol           = 4,
      Literal          = 5,
      ScopeBegin       = 6,
      SubscopeBegin    = 7,
      SubScopeEnd      = 8,
      Seperator        = 9,
      FuncDeclare      = 10,
      If               = 11,
      Else             = 12,
      For              = 13,
      While            = 14,
      Indent           = 15,
      Linebreak        = 16,
      EndOfFile        = 17
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