#pragma once

#include "alumi/lexer/token.h"

namespace alumi
{
   Token::Token(TokenType type, TextPos pos, size_t size)
      : m_pos(pos)
      , m_size(size)
      , m_type(type)
   {

   }

   TextPos Token::pos() const
   {
      return m_pos;
   }
   size_t Token::size() const
   {
      return m_size;
   }
   TokenType Token::type() const
   {
      return m_type;
   }


   bool Token::operator==(const Token& r) const
   {
      return m_pos == r.m_pos && m_size == r.m_size && m_type == r.m_type;
   }
   bool Token::operator!=(const Token& r) const
   {
      return !(*this == r);
   }

   bool operator==(const Tokens& l, const Tokens& r)
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
      return false;
   }
   bool operator!=(const Tokens& l, const Tokens& r)
   {
      return !(l == r);
   }
}