module;

#include "alumi/lexer/token.h"

export module alumi.parser.combinator:is;

import :concepts;

import alumi.parser.result;
import alumi.parser.subparser;

using namespace alumi;

export {

   template<TokenType token_type>
   class Is
   {
   public:
      static Result parse(Subparser& parent)
      {
         if (parent.advance().type() == token_type)
         {
            return Result(Result::Type::Success, parent, {});
         }
         parent.do_panic(parent.current_token_index() - 1);
         return Result(Result::Type::Failure, parent, {});
      }
   private:
   };

}