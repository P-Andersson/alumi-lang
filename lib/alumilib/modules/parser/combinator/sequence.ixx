export module alumi.parser.combinator:sequence;

import :concepts;

import alumi.parser.result;
import alumi.parser.subparser;

export {

   template<ParserElement... Ts>
   class Sequence
   {
   public:
      static Result parse(Subparser& parent)
      {
         State state;
         Result res = do_parse<Ts...>(parent, state);
         return Result(state.worst_result, parent, state.child_nodes);
      }
   private:
      class State
      {
      public:
         Result::Type worst_result = Result::Type::Success;
         Nodes child_nodes;
      };

      template<ParserElement ElemT>
      static Result do_parse(Subparser& parent, State& state)
      {
         Result res = ElemT::parse(parent);
         if (res.get_type() < state.worst_result)
         {
            state.worst_result = res.get_type();
         }

         state.child_nodes.insert(state.child_nodes.end(), res.get_nodes().begin(), res.get_nodes().end());

         return res;
      }

      template<ParserElement ElemT, ParserElement NextT, ParserElement... OthersT>
      static Result do_parse(Subparser& parent, State& state)
      {
         Result res = do_parse<ElemT>(parent, state);

         if (res.get_type() != Result::Type::Failure)
         {
            return do_parse<NextT, OthersT...>(parent, state);
         }
         else
         {
            return Result(state.worst_result, parent, state.child_nodes);
         }
      }

   };

   //!

}