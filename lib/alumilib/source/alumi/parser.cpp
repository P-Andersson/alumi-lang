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
               return Node(Error(res.get_nodes()), res);
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
               return Node(Error(res.get_nodes()), res);
            }
            return Node(syntax_tree::FunctionDefinition(), res);

         };
         RULE(FuncDeclare, ParseRule<
            Sequence<Is<TokenType::FuncDeclare>, 
                     Is<TokenType::SubscopeBegin>,
                     FuncParameters,
                     Is<TokenType::SubScopeEnd>,
                     Optional<Sequence<Is<TokenType::ReturnOp>, Is<TokenType::Symbol>>>, 
                     Is<TokenType::ScopeBegin>, Is<TokenType::Linebreak>
            >, SynchronizeOnToken<TokenType::Linebreak>, build_func_decleration>);
         
         std::optional<Node> build_expression_node(const ParseResult& res)
         {
            if (res.get_type() == ParseResult::Type::Failure)
            {
               return Node(Error(res.get_nodes()), res);
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
               return Node(Error(res.get_nodes()), res);
            }
            return Node(syntax_tree::Assignment(), res);
         };

         RULE(Assignment, 
            ParseRule<Sequence<
               Is<TokenType::Symbol>, 
               Is<TokenType::Assignment>,
               AnyOf<
                  Sequence<FuncDeclare, CodeBlock>,
                  Expression
                  >,
               EndOfLine
            >, SynchronizeOnToken<TokenType::Assignment>, build_assignment_node>);

         std::optional<Node> build_statement_node(const ParseResult& res)
         {
            if (res.get_type() == ParseResult::Type::Failure)
            {
               return Node(Error(res.get_nodes()), res);
            }
            return Node(syntax_tree::Statement(), res);
         };
         RULE(Statement, ParseRule<
            AnyOf<
            Is<TokenType::Noop>,
            Assignment
            >, SynchronizeOnToken<TokenType::Linebreak>, build_statement_node>);

         std::optional<Node> build_block_node(const ParseResult& res)
         {
            if (res.get_type() == ParseResult::Type::Failure)
            {
               return Node(Error(res.get_nodes()), res);
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
               return Node(Error(res.get_nodes()), res);
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

      ParseResult AlumiParser::parse(const std::vector<Token>& tokens) const
      {
         Subparser root_parser(tokens);
         return Root::parse(root_parser);
      }
   }
}