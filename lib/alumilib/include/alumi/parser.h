#pragma once

#include <memory>

#include "alumi/lexer.h"
#include "alumi/syntax_tree.h"

namespace alumi
{
   namespace parser
   {
      class AlumiParser
      {
      public:
         AlumiParser();
         ~AlumiParser();

         SyntaxTree parse(const LexedText& text) const;

      private:
      };
   }
}