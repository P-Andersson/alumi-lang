#pragma once

#include "alumi/syntax_tree/tree_nodes.h"

#include "alumi/lexer/token.h"
#include "alumi/parser/data.h"

#include <string>

namespace alumi
{
   namespace syntax_tree
   {
      class TreeOpRepresentTree
      {
      public:
         std::string representation;

         TreeOpRepresentTree(const std::vector<UnicodeCodePoint>* text, const std::vector<Token>* tokens);

         void operator()(ModuleTreeWalker& walker, Node& node);
      private:
         const std::vector<UnicodeCodePoint>* m_text;
         const std::vector<Token>* m_tokens;
         size_t m_index = 0;
      };
   }
}