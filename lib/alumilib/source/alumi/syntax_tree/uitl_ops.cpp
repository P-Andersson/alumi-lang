#include "alumi/syntax_tree/util_ops.h"

#include "alumi/syntax_tree/nodes.h"

#include <vector>

namespace alumi
{
   namespace syntax_tree
   {
      namespace
      {
         std::string to_string(const std::tuple<TextPos, TextPos>& span)
         {
            auto [start, end] = span;
            return "(L:" + std::to_string(start.line()) + ", C:" + std::to_string(start.col()) + ")";
         }

         class NodeOpStringify
         {
         public:
            std::string operator()(ModuleRoot& node) const
            {
               return "ModuleRoot";
            }
            std::string operator()(Error& node) const
            {
               return "Error";
            }
            std::string operator()(Declaration& node) const
            {
               return "Declaration";
            }
            std::string operator()(IntegerLiteral& node) const
            {
               return "IntegerLiteral";
            }
            std::string operator()(Expression& node) const
            {
               return "Expression";
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

      void TreeOpRepresentTree::operator()(ModuleTreeWalker& walker, Node& node) 
      {
         size_t in_index = m_index;
         std::string name;
         for (size_t i = 0; i < in_index; ++i)
         {
            name += "   ";
         }
         name += to_string(node.spans_text()) + " " + node.visit(NodeOpStringify());
         representation += name + "\n";

         m_index += 1;
         for (size_t i = 0; i < node.child_group_count(); ++i)
         {
            walker.walk(*this, node.child_group(i));
         }
         m_index = in_index;
      }
   }
}