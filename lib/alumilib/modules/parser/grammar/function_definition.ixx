module;

#include "alumi/parser/grammar/rule_macro.h"
#include "alumi/lexer/token.h"

#include <cassert>
#include <optional>

export module alumi.parser.grammar:function_definition;

import alumi.parser.error_codes;
import alumi.syntax_tree.nodes;
import alumi.syntax_tree.node_builder;

using namespace alumi;

std::optional<Node> build_func_definition(const Result& res)
{
   if (res.get_type() == Result::Type::Failure)
   {
      Error error(ErrorCode::Unknown, *res.get_panic_token_index(), res.get_nodes());
      return make_node(error, res);
   }
   const auto& dec = res.get_nodes()[0];
   return make_node(FunctionDefinition(dec, res.get_nodes()[dec.recursive_child_count()]), res);

};
