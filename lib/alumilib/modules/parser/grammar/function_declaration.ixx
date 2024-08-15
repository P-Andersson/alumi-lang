module;

#include "alumi/parser/grammar/rule_macro.h"
#include "alumi/lexer/token.h"

#include <cassert>
#include <optional>

export module alumi.parser.grammar:function_declaration;
import alumi.syntax_tree.nodes;
import alumi.syntax_tree.node_builder;

using namespace alumi;

std::optional<Node> build_function_parameters_scope(const Result& res)
{
   if (res.get_type() == Result::Type::Failure)
   {
      return make_node(Error(ErrorCode::Unknown, *res.get_panic_token_index(), res.get_nodes()), res);
   }
   assert(res.get_nodes().size() == 1);

   return res.get_nodes()[0];
};

std::optional<Node> build_function_decleration(const Result& res)
{
   if (res.get_type() == Result::Type::Failure)
   {
      return make_node(Error(ErrorCode::Unknown, *res.get_panic_token_index(), res.get_nodes()), res);
   }
   std::optional<Node> return_type = std::nullopt;
   if (res.get_nodes().size() > 1)
   {
      return_type.emplace(res.get_nodes()[1]);
   }

   return make_node(FunctionDecleration(res.get_nodes()[0], return_type), res);

};
