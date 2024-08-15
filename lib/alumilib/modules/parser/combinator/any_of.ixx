module;

#include <optional>

export module alumi.parser.combinator:any_of;

import :concepts;

import alumi.parser.result;
import alumi.parser.subparser;

export {

   //! 
   //! Expects one of several child-rules. If none of them matches, produces a parse failure on the LONGEST match before failure. In case of a tie, fails on the last subrule of those tied
   //! @tparam Ts    one or more child rules
   //! 
   template<ParserElement... Ts>
   class AnyOf
   {
   public:
      static Result parse(Subparser& parent)
      {
         std::optional<Result> best_failure;
         Result res = do_parse<Ts...>(parent, best_failure);
         if (res.get_type() != Result::Type::Success)
         {
            res = *best_failure;
         }
         parent.take_over_from(res.get_subparser());
         return Result(res.get_type(), parent, res.get_nodes());

      }
   private:

      template<ParserElement ElemT>
      static Result do_parse(Subparser& parent, std::optional<Result>& best_failure)
      {
         Subparser parser = parent.create_child();
         Result res = ElemT::parse(parser);
         if (res.get_type() != Result::Type::Success)
         {
            if (!best_failure.has_value() ||
               (res.get_type() > best_failure->get_type()) ||
               (res.get_type() == best_failure->get_type() && res.get_consumed() > best_failure->get_consumed()))
            {
               best_failure = res;
            }
         }
         return res;
      }

      template<ParserElement ElemT, ParserElement NextT, ParserElement... OthersT>
      static Result do_parse(Subparser& parent, std::optional<Result>& best_failure)
      {
         Result res = do_parse<ElemT>(parent, best_failure);
         if (res.get_type() != Result::Type::Success)
         {
            return do_parse<NextT, OthersT...>(parent, best_failure);
         }
         else
         {
            return res;
         }
      }

   };

}