module;

#include "alumi/lexer/token.h"

#include <optional>

export module alumi.parser.combinator:no_indent;

import :concepts;

import alumi.parser.result;
import alumi.parser.subparser;

using namespace alumi;

export {

   class NoIndent
   {
   public:
      static Result parse(Subparser& parent)
      {
         auto indent = parent.get_indent();
         auto token = parent.advance();

         if (token.type() == TokenType::Indent)
         {
            if (indent == parent.get_indent())
            {
               return Result(Result::Type::Success, parent, {});
            }
         }
         parent.do_panic(parent.current_token_index() - 1);
         return Result(Result::Type::Failure, parent, {});
      }
   private:
   };

}
