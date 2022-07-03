#pragma once

#include "alumi/syntax_tree/nodes.h"

#include "alumi/parser/parser_parts.h"
#include "alumi/lexer/lexed_text.h"

namespace alumi
{

   //!
   //! The Snytax Tree created by a an alumi-parser. Note that the trees nodes may not be the same as the parse results nodes,
   //! as the trees nodes may have been modified
   //! 
   class SyntaxTree
   {
   public:
      SyntaxTree(const parser::ParseResult& result, const LexedText* source);

      syntax_tree::Nodes& nodes();
      const syntax_tree::Nodes& nodes() const;

      const parser::ParseResult& parse_result() const;
      const LexedText& source() const;
   
      //!
      //! Gets the direct token indices that a single node is built of. Note that the result may be empty
      //! for nodes who did not consume tokens directly, just other nodes
      //! 
      std::vector<size_t> direct_tokens(const syntax_tree::Node* node) const;

   private:
      parser::ParseResult m_result;

      syntax_tree::Nodes m_current_nodes;
      const LexedText* m_source;

   };
}