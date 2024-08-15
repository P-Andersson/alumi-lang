#include "alumi/syntax_tree/tree_utiltiy_ops.h"

#include <vector>

import alumi.syntax_tree;

namespace alumi:syntax_tree
{
namespace
{
   class NodeOpStringify
   {
   public:
      std::string operator()(ModuleRoot& node) const
      {
         return "ModuleRoot";
      }
      std::string operator()(CodeBlock& node) const
      {
         return "CodeBlock";
      }
      std::string operator()(Error& node) const
      {
         return "Error";
      }
      std::string operator()(Assignment& node) const
      {
         return "Assignment";
      }
      std::string operator()(IntegerLiteral& node) const
      {
         return "IntegerLiteral";
      }
      std::string operator()(Expression& node) const
      {
         return "Expression";
      }
      std::string operator()(Statement& node) const
      {
         return "Statement";
      }
      std::string operator()(FunctionDecleration& node) const
      {
         return "FunctionDecleration";
      }
      std::string operator()(FunctionDefinition& node) const
      {
         return "FunctionDefinition";
      }
      std::string operator()(FunctionCall& node) const
      {
         return "FunctionCall";
      }
      std::string operator()(Brancher& node) const
      {
         return "Brancher";
      }
   };
}

TreeOpRepresentTree::TreeOpRepresentTree(const SyntaxTree* tree)
   : m_tree(tree)
{

}

void TreeOpRepresentTree::operator()(SyntaxTreeWalker& walker, Node& node) 
{
   size_t in_index = m_index;
   std::string name;
   for (size_t i = 0; i < in_index; ++i)
   {
      name += "   ";
   }
   name += node.visit(NodeOpStringify());
   representation += name + "(\"";

   auto code_points = as_string(node, *m_tree);
   for (auto c : code_points)
   {
      if (c != '\n')
      {
         representation += as_utf8(c);
      }
      else
      {
         representation += "\\n";
      }
   }

   representation += "\")\n";


   m_index += 1;
   for (size_t i = 0; i < node.child_group_count(); ++i)
   {
      walker.walk(*this, node.child_group(i));
   }
   m_index = in_index;
}
}