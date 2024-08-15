module;

#include "alumi/parser/grammar/rule_macro.h"

#include "alumi/lexer/token.h"

#include <optional>

export module alumi.parser.grammar:function_parameter;

import alumi.syntax_tree.nodes;
import alumi.syntax_tree.node_builder;

using namespace alumi;


std::optional<Node> build_function_parameters_node(const Result& res)
{
   if (res.get_type() == Result::Type::Failure)
   {
      return make_node(Error(ErrorCode::Unknown, *res.get_panic_token_index(), res.get_nodes()), res);
   }
   return make_node(Expression(), res);
};

