#pragma once

#include "alumi/lexer/lexed_text.h"

import alumi.syntax_tree.nodes;
import alumi.parser.result;

namespace alumi
{

   //!
   //! The Snytax Tree created by a an alumi-parser. Note that the trees nodes may not be the same as the parse results nodes,
   //! as the trees nodes may have been modified
   //! 
   class SyntaxTree
   {
   public:
      SyntaxTree(const Result& result, const LexedText* source);

      Nodes& nodes();
      const Nodes& nodes() const;

      const Result& parse_result() const;
      const LexedText& source() const;
   
      //!
      //! Gets the direct token indices that a single node is built of. Note that the result may be empty
      //! for nodes who did not consume tokens directly, just other nodes
      //! 
      std::vector<size_t> direct_tokens(const Result* node) const;

   private:
      Result m_result;

      Nodes m_current_nodes;
      const LexedText* m_source;

   };
}