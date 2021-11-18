#pragma once

#include <memory>

#include "alumi/lexer.h"
#include "alumi/parser/parser_parts.h"

namespace alumi
{
   namespace parser
   {
      class AlumiParser
      {
      public:
         AlumiParser();
         ~AlumiParser();

         ParseResult parse(const std::vector<Token>& tokens) const;

      private:
      };
   }
}