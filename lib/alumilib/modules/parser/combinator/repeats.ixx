export module alumi.parser.combinator:repeats;

import :concepts;

import alumi.parser.result;
import alumi.parser.subparser;

export {

   template<ParserElement T>
   class Repeats
   {
   public:
      static Result parse(Subparser& parent)
      {

         Nodes child_nodes;
         while (true)
         {
            Subparser parser = parent.create_child();
            Result res = T::parse(parser);
            if (res.get_type() == Result::Type::Failure)
            {
               return Result(Result::Type::Success, parent, child_nodes);
            }
            else
            {
               child_nodes.insert(child_nodes.end(), res.get_nodes().begin(), res.get_nodes().end());
               parent.take_over_from(res.get_subparser());
            }
         }
      }

   private:
   };

}