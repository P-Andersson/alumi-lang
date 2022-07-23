module;

#include "alumi/parser/grammar/rule_macro.h"
#include "alumi/lexer/token.h"

#include <cassert>

export module alumi.parser.grammar:function_declaration;

using namespace alumi;
using namespace alumi::parser;
using namespace alumi::syntax_tree;

std::optional<Node> build_function_parameters_scope(const ParseResult& res)
{
   if (res.get_type() == ParseResult::Type::Failure)
   {
      return Node(Error(ErrorCode::Unknown, *res.get_panic_token_index(), res.get_nodes()), res);
   }
   assert(res.get_nodes().size() == 1);

   return res.get_nodes()[0];
};

std::optional<Node> build_function_decleration(const ParseResult& res)
{
   if (res.get_type() == ParseResult::Type::Failure)
   {
      return Node(Error(ErrorCode::Unknown, *res.get_panic_token_index(), res.get_nodes()), res);
   }
   std::optional<Node> return_type = std::nullopt;
   if (res.get_nodes().size() > 1)
   {
      return_type.emplace(res.get_nodes()[1]);
   }

   return Node(syntax_tree::FunctionDecleration(res.get_nodes()[0], return_type), res);

};
