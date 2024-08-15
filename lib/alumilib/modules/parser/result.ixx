module;

#include <cassert>

export module alumi.parser.result;

import alumi.parser.subparser;
import alumi.syntax_tree.nodes;

namespace alumi::parser {

//!
//! Describes the results of a specific parsing attempt
//! 
export class Result
{
public:
   enum class Type
   {
      Failure,
      RecoveredFailure,
      Success
   };

   Result(Type type,
          const Subparser& parser,
          const Nodes& parsed_nodes)
      : m_type(type)
      , m_parser_used(parser)
      , m_nodes(parsed_nodes)
   {
      assert(type != Result::Type::Failure || m_parser_used.is_panicing());
   }

   Type get_type() const
   {
      return m_type;
   }

   const Subparser& get_subparser() const
   {
      return m_parser_used;
   }

   size_t get_consumed() const
   {
      return m_parser_used.get_distance();
   }

   //!
   //! Returns the token that caused this result to panic, or std::nullopt if none
   //! 
   std::optional<size_t> get_panic_token_index() const
   {
      return m_parser_used.get_panic_token_index();
   }

   const Nodes& get_nodes() const
   {
      return m_nodes;
   }

private:
   Type m_type;

   Subparser m_parser_used;

   Nodes m_nodes;

};

}
