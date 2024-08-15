export module alumi.parser.combinator:optional;

import :concepts;

import alumi.parser.result;
import alumi.parser.subparser;

export {

   template<ParserElement T>
   class Optional
   {
   public:
      static Result parse(Subparser& parent)
      {
         Subparser parser = parent.create_child();
         auto res = T::parse(parser);
         if (res.get_type() == Result::Type::Failure)
         {
            return Result(Result::Type::Success, parent, {});
         }
         else
         {
            parent.take_over_from(res.get_subparser());
            return Result(Result::Type::Success, parent, res.get_nodes());
         }
      }

   private:
   };

}