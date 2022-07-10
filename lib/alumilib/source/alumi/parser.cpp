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
         class CodeBlock;

         RULE(NewBlock, Indented);
         RULE(EndBlock, AnyOf<Is<TokenType::EndOfFile>, Sequence<Is<TokenType::Linebreak>, Dedented>>);
         RULE(EndOfLine, AnyOf<Is<TokenType::Linebreak>, Is<TokenType::EndOfFile>>);

         std::optional<Node> build_func_parameters_prototype(const ParseResult& res)
         {
            if (res.get_type() == ParseResult::Type::Failure)
            {
               return Node(Error(*res.get_panic_token_index(), res.get_nodes()), res);
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
               return Node(Error(*res.get_panic_token_index(), res.get_nodes()), res);
            }
            std::optional<Node> return_type = std::nullopt;
            if (res.get_nodes().size() > 1)
            {
               return_type.emplace(res.get_nodes()[1]);
            }

            return Node(syntax_tree::FunctionDecleration(res.get_nodes()[0], return_type), res);

         };
         RULE(FuncDeclare, ParseRule<
            Sequence<Is<TokenType::FuncDeclare>, 
                     Is<TokenType::SubscopeBegin>,
                     FuncParameters,
                     Is<TokenType::SubScopeEnd>,
                     Optional<Sequence<Is<TokenType::ReturnOp>, Is<TokenType::Symbol>>>, 
                     Is<TokenType::ScopeBegin>, Is<TokenType::Linebreak>
            >, SynchronizeOnToken<TokenType::Linebreak>, build_func_decleration>);

         std::optional<Node> build_func_definition(const ParseResult& res)
         {
            if (res.get_type() == ParseResult::Type::Failure)
            {
               return Node(Error(*res.get_panic_token_index(), res.get_nodes()), res);
            }
            const auto& dec = res.get_nodes()[0];
            return Node(syntax_tree::FunctionDefinition(dec, res.get_nodes()[dec.recursive_child_count()]), res);

         };
         RULE(FuncDefine, ParseRule<
            Sequence<FuncDeclare, CodeBlock>, SynchronizeOnToken<TokenType::Linebreak>, build_func_definition>);
         
         std::optional<Node> build_expression_node(const ParseResult& res)
         {
            if (res.get_type() == ParseResult::Type::Failure)
            {
               return Node(Error(*res.get_panic_token_index(), res.get_nodes()), res);
            }
            return Node(syntax_tree::Expression(), res);
         };
         RULE(Expression, ParseRule<
               AnyOf<
                  Is<TokenType::Literal>,
                  Is<TokenType::Symbol>
               >, NeverSynchroize, build_expression_node>);

         std::optional<Node> build_assignment_node(const ParseResult& res)
         {
            if (res.get_type() == ParseResult::Type::Failure)
            {
               return Node(Error(*res.get_panic_token_index(), res.get_nodes()), res);
            }
            return Node(syntax_tree::Assignment(res.get_nodes()[0]), res);
         };

         RULE(Assignment, 
            ParseRule<Sequence<
               Is<TokenType::Symbol>, 
               Is<TokenType::Assignment>,
               AnyOf<
                  FuncDefine,
                  Expression
                  >,
               EndOfLine
            >, SynchronizeOnToken<TokenType::Linebreak>, build_assignment_node>);

         std::optional<Node> build_statement_node(const ParseResult& res)
         {
            if (res.get_type() == ParseResult::Type::Failure)
            {
               return Node(Error(*res.get_panic_token_index(), res.get_nodes()), res);
            }
            return Node(syntax_tree::Statement(res.get_nodes()), res);
         };
         RULE(Statement, ParseRule<
            AnyOf<
            Is<TokenType::Noop>,
            Assignment
            >, NeverSynchroize, build_statement_node>);

         std::optional<Node> build_block_node(const ParseResult& res)
         {
            if (res.get_type() == ParseResult::Type::Failure)
            {
               return Node(Error(*res.get_panic_token_index(), res.get_nodes()), res);
            }
            return Node(syntax_tree::CodeBlock(res.get_nodes()), res);
         };
         RULE(CodeBlock, ParseRule<
            Sequence<
               NewBlock,
               Statement,
               EndOfLine,
               Repeats<Sequence<NoIndent, Statement, EndOfLine>>,
               EndBlock
            >, SynchronzieOnMatchedPair<TokenType::Indent, TokenType::Indent>, build_block_node>);


         std::optional<Node> build_root_node(const ParseResult& res)
         {
            if (res.get_type() == ParseResult::Type::Failure)
            {
               return Node(Error(*res.get_panic_token_index(), res.get_nodes()), res);
            }
            return Node(ModuleRoot(res.get_nodes()), res);
         };
        
         RULE(Root, ParseRule<AnyOf<EndOfLine, CodeBlock>, SynchronizeOnToken<TokenType::Linebreak>, build_root_node>);
      }


      AlumiParser::AlumiParser()
      {

      }
      AlumiParser::~AlumiParser()
      {

      }

      SyntaxTree AlumiParser::parse(const LexedText& text) const
      {
         Subparser root_parser(text.tokens());
         return SyntaxTree(Root::parse(root_parser), &text);
      }
   }
}