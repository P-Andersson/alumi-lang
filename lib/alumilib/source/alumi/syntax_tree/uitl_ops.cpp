#include "alumi/syntax_tree/util_ops.h"

#include "alumi/syntax_tree/nodes.h"

#include <vector>

namespace alumi
{
   namespace syntax_tree
   {
      namespace
      {
         /*std::string to_string(const std::tuple<size_t, size_t>& span)
         {
            auto [start, end] = span;
            return "(L:" + std::to_string(start.line()) + ", C:" + std::to_string(start.col()) + ")";
         }*/

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

      TreeOpRepresentTree::TreeOpRepresentTree(const std::vector<UnicodeCodePoint>* text, const std::vector<Token>* tokens)
         : m_text(text)
         , m_tokens(tokens)
      {

      }

      void TreeOpRepresentTree::operator()(ModuleTreeWalker& walker, Node& node) 
      {
         size_t in_index = m_index;
         std::string name;
         for (size_t i = 0; i < in_index; ++i)
         {
            name += "   ";
         }
         name += node.visit(NodeOpStringify());
         representation += name + "(\"";

         auto [start, end] = node.spans_tokens();
         if (node.child_group_count() > 0 && node.child_group(0).get_count() > 0)
         {
            end = start + node.child_group(0).get_skips() + 1;
         }
         for (size_t char_i = (*m_tokens)[start].pos().char_index(); char_i < std::min(m_text->size(), (*m_tokens)[end - 1].pos().char_index() + (*m_tokens)[end - 1].size()); ++char_i)
         {
            auto c = (*m_text)[char_i];
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
}