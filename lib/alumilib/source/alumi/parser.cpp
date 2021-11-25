#include "alumi/parser.h"

#include "alumi/parser/parser_parts.h"
#include "alumi/parser/parse_rule.h"

using namespace alumi::syntax_tree;

#define RULE(name, ...) class name; class name  { public: using RuleOp = __VA_ARGS__;  static ParseResult parse(Subparser& parent){ return RuleOp::parse(parent); }; }

namespace alumi
{
   namespace parser
   {
      namespace {
         RULE(NewBlock, Indented);
         RULE(EndBlock, AnyOf<Is<TokenType::EndOfFile>, Sequence<Is<TokenType::Linebreak>, Dedented>>);
         RULE(EndOfLine, AnyOf<Is<TokenType::Linebreak>, Is<TokenType::EndOfFile>>);

         std::optional<Node> build_func_parameters_prototype(const ParseResult& res)
         {
            if (res.get_type() == ParseResult::Type::Failure)
            {
               return Node(Error(), res);
            }
            return Node(syntax_tree::Expression(), res);
         };
         RULE(FuncParameters, ParseRule<
            RepeatsWithSeperator<Sequence<Is<TokenType::Symbol>, 
                     Is<TokenType::Symbol>>, TokenType::Seperator>
            , NeverSynchroize, build_func_parameters_prototype>);

         std::optional<Node> build_func_decleration(const ParseResult& res)
         {
            if (res.get_type() == ParseResult::Type::Failure)
            {
               return Node(Error(), res);
            }
            return Node(syntax_tree::FunctionDefinition(), res);
         };
         RULE(FuncDeclare, ParseRule<
            Sequence<Is<TokenType::FuncDeclare>, 
                     Is<TokenType::SubscopeBegin>,
                     FuncParameters,
                     Is<TokenType::SubScopeEnd>
            >, NeverSynchroize, build_func_decleration>);
         
         std::optional<Node> build_expression_node(const ParseResult& res)
         {
            if (res.get_type() == ParseResult::Type::Failure)
            {
               return Node(Error(), res);
            }
            return Node(syntax_tree::Expression(), res);
         };
         RULE(Expression, ParseRule<
               AnyOf<
                  Is<TokenType::Noop>
               >, NeverSynchroize, build_expression_node>);


         std::optional<Node> build_block_node(const ParseResult& res)
         {
            if (res.get_type() == ParseResult::Type::Failure)
            {
               return Node(Error(), res);
            }
            return Node(ModuleRoot(res.get_nodes()), res);
         };
         RULE(CodeBlock, ParseRule<
            Sequence<
               NewBlock,
               Expression,
               EndOfLine,
               Repeats<Sequence<NoIndent, Expression, EndOfLine>>,
               EndBlock
            >, SynchronzieOnMatchedPair<TokenType::Indent, TokenType::Indent>, build_block_node>);

         std::optional<Node> build_function_define_node(const ParseResult& res)
         {
            if (res.get_type() == ParseResult::Type::Failure)
            {
               return Node(Error(), res);
            }
            return Node(syntax_tree::FunctionDefinition(), res);
         };

         RULE(FunctionDefine, ParseRule <
            Sequence<Optional<Sequence<Is<TokenType::ReturnOp>, Is<TokenType::Symbol>>>, Is<TokenType::ScopeBegin>, Is<TokenType::Linebreak>>, SynchronizeOnToken<TokenType::Linebreak>, build_function_define_node >);

         std::optional<Node> build_declare_node(const ParseResult& res)
         {
            if (res.get_type() == ParseResult::Type::Failure)
            {
               return Node(Error(), res);
            }
            return Node(Declaration(), res);
         };

         RULE(Declare, 
            ParseRule<Sequence<Is<TokenType::Symbol>, 
            Is<TokenType::Assignment>,
            AnyOf<
               Sequence<FuncDeclare, FunctionDefine, CodeBlock>
               >,
            EndOfLine
            >, SynchronizeOnToken<TokenType::EndOfFile>, build_declare_node>);

         std::optional<Node> build_root_node(const ParseResult& res)
         {
            if (res.get_type() == ParseResult::Type::Failure)
            {
               return Node(Error(), res);
            }
            return Node(ModuleRoot(res.get_nodes()), res);
         };

         RULE(Root, ParseRule<
            AnyOf<
            EndOfLine,
               Sequence<Indented, Declare, Repeats<Sequence<NoIndent, Declare>>>
            >, SynchronizeOnToken<TokenType::Linebreak>, build_root_node>);
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
         return Root::parse(root_parser);
      }
   }
}