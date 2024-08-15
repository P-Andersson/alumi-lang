export module alumi.parser.combinator:peek;

import :concepts;

import alumi.parser.result;
import alumi.parser.subparser;

export {

   template<ParserElement T>
   class Peek
   {
   public:
      static Result parse(Subparser& parent)
      {
         Subparser child = parent.create_child();
         auto res = T::parse(child);
         if (res.get_type() == Result::Type::Success)
         {
            return Result(Result::Type::Success, parent, {});
         }
         parent.do_panic(parent.current_token_index());
         return Result(Result::Type::Failure, parent, {});
      }
   private:
   };

}