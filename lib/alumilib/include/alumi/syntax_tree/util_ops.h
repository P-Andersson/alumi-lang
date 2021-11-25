#pragma once

#include "alumi/syntax_tree/tree_nodes.h"

#include <string>

namespace alumi
{
   namespace syntax_tree
   {
      class TreeOpRepresentTree
      {
      public:
         std::string representation;

         void operator()(ModuleTreeWalker& walker, Node& node);
      private:
         size_t m_index = 0;
      };
   }
}