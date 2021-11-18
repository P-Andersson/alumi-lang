#include "alumi/parser.h"

#include "alumi/parser/parser_parts.h"
#include "alumi/parser/parse_rule.h"

using namespace alumi::syntax_tree;

#define RULE(name, ...) class name; class name : public __VA_ARGS__ {}

namespace alumi
{
   namespace parser
   {
      namespace {
         std::optional<Node> produce_error_node(const ParseResult& res)
         {
            if (res.get_type() == ParseResult::Type::Failure)
            {
               return Node(Error(), res);
            }
            return std::nullopt;
         };

         RULE(func_parameters, ParseRule<
            RepeatsWithSeperator<Sequence<Is<TokenType::Symbol>, 
                     Is<TokenType::Symbol>>, TokenType::Seperator>
            , NeverSynchroize, produce_error_node>);
         RULE(func_declare, ParseRule<
            Sequence<Is<TokenType::FuncDeclare>, 
                     Is<TokenType::SubscopeBegin>,
                     func_parameters,
                     Is<TokenType::SubScopeEnd>,
                     Optional<Sequence<Is<TokenType::ReturnOp>, Is<TokenType::Symbol>>
                     >
            >, NeverSynchroize, produce_error_node>);
         
         RULE(expression, ParseRule<
               AnyOf<
                  Is<TokenType::Noop>
               >, NeverSynchroize, produce_error_node>);
         RULE(code_block, ParseRule<
            Sequence<
               Is<TokenType::ScopeBegin>,
               Indented,
               expression,
               Repeats<Sequence<NoIndent, expression>>,
               Optional<Peek<Dedented>>
            >, NeverSynchroize, produce_error_node>);

         RULE(declare, ParseRule<Sequence<Is<TokenType::Symbol>, Is<TokenType::Assignment>,
            AnyOf<
               Sequence<func_declare, code_block>
               >
            >, NeverSynchroize, produce_error_node>);

         std::optional<Node> build_root_node(const ParseResult& res)
         {
            if (res.get_type() == ParseResult::Type::Failure)
            {
               return Node(Error(), res);
            }
            return Node(ModuleRoot(), res);
         };

         RULE(root, ParseRule<
            AnyOf<
               Is<TokenType::EndOfFile>,
               Sequence<Indented, declare, Repeats<Sequence<NoIndent, declare>>, Is<TokenType::EndOfFile>>
            >, SynchronizeOnToken<TokenType::EndOfFile>, produce_error_node>);
      }


      AlumiParser::AlumiParser()
      {

      }
      AlumiParser::~AlumiParser()
      {

      }

      ParseResult AlumiParser::parse(const std::vector<Token>& tokens) const
      {
         Subparser root_parser(tokens);
         return root::parse(root_parser);
      }
   }
}