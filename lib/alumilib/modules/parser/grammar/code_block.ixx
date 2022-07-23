module;

#include "alumi/parser/grammar/rule_macro.h"
#include "alumi/lexer/token.h"

#include <cassert>

export module alumi.parser.grammar:code_block;

using namespace alumi;
using namespace alumi::parser;

using alumi::syntax_tree::Node;
using alumi::syntax_tree::Error;

std::optional<Node> build_block_node(const ParseResult& res)
{
   if (res.get_type() == ParseResult::Type::Failure)
   {
      return Node(Error(ErrorCode::Unknown, *res.get_panic_token_index(), res.get_nodes()), res);
   }
   return Node(syntax_tree::CodeBlock(res.get_nodes()), res);
};
