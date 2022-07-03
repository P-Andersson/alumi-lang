#pragma once

#include "alumi/syntax_tree/tree_nodes.h"

#include "alumi/syntax_tree.h"
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

         TreeOpRepresentTree(const SyntaxTree* tree);

         void operator()(SyntaxTreeWalker& walker, Node& node);
      private:
         const SyntaxTree* m_tree;
         size_t m_index = 0;
      };
   }
}