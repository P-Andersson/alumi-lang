#include "catch2/catch.hpp"

#include "alumi/parser/parse_rule.h"

using namespace alumi;
using namespace alumi::parser;
using namespace alumi::syntax_tree;


namespace {

   Node build_node(const ParseResult& res)
   {
      if (res.get_type() == ParseResult::Type::Failure)
      {
         return Node(Error());
      }
      return Node(IntegerLiteral());
   };

   Node build_node2(const ParseResult& res)
   {
      if (res.get_type() == ParseResult::Type::Failure)
      {
         return Node(Error());
      }
      return Node(Expression());
   };
   Node build_node3(const ParseResult& res)
   {
      if (res.get_type() == ParseResult::Type::Failure)
      {
         return Node(Error());
      }
      return Node(FunctionCall());
   };


   TEST_CASE("Test Parse Rule - Simple Sequence")
   {
      SECTION("Simple Sequence")
      {
         auto rule = ParseRule<Sequence<Is<TokenType::Indent>, Is<TokenType::Literal>>, build_node>();

         SECTION("Ok")
         {

            std::vector<Token> tokens{
               Token(TokenType::Indent, TextPos(0, 0), 2),
               Token(TokenType::Literal, TextPos(0, 2), 2)
            };
            Subparser parser(tokens);

            auto res = rule.parse(parser);
            REQUIRE(res.get_type() == ParseResult::Type::Success);
            REQUIRE(res.get_consumed() == 2);

            REQUIRE(res.get_nodes().size() == 1);
            REQUIRE(res.get_nodes().at(0).is<IntegerLiteral>());
         }
         SECTION("Fail")
         {

            std::vector<Token> tokens{
               Token(TokenType::Indent, TextPos(0, 0), 2),
               Token(TokenType::Indent, TextPos(0, 2), 2)
            };
            Subparser parser(tokens);

            auto res = rule.parse(parser);
            REQUIRE(res.get_type() == ParseResult::Type::Failure);
            REQUIRE(res.get_consumed() == 2);

            REQUIRE(res.get_nodes().size() == 1);
            REQUIRE(res.get_nodes().at(0).is<Error>());
         }
      }
   }

   class Rule3;
   class Rule2;
   class Branch;
   class Rule1;

   class Rule3 : public ParseRule < Sequence<Is<TokenType::Operator>, Is<TokenType::Operator>>, build_node3> {};
   class Rule2 : public ParseRule < Sequence< Is<TokenType::Symbol>, Optional<Rule2>>, build_node2> {};
   class Branch : public AnyOf<Rule2, Rule3>{};
   class Rule1 : public ParseRule<Sequence< Branch>, build_node> {};

   TEST_CASE("Test Parse Rule - Complex Test")
   {
      SECTION("Sequene followed by Branch")
      {
         auto rule = Rule1();

         SECTION("Ok - Branch 1")
         {

            std::vector<Token> tokens{
               Token(TokenType::Operator, TextPos(0, 0), 2),
               Token(TokenType::Operator, TextPos(0, 2), 2),
            };
            Subparser parser(tokens);

            auto res = rule.parse(parser);
            REQUIRE(res.get_type() == ParseResult::Type::Success);
            REQUIRE(res.get_consumed() == 2);

            REQUIRE(res.get_nodes().size() == 2);
            REQUIRE(res.get_nodes().at(0).is<IntegerLiteral>());
            REQUIRE(res.get_nodes().at(1).is<FunctionCall>());
         }
         SECTION("Ok - Branch 2 Recurse")
         {

            std::vector<Token> tokens{
               Token(TokenType::Symbol, TextPos(0, 0), 2),
               Token(TokenType::Symbol, TextPos(0, 2), 2),
               Token(TokenType::Symbol, TextPos(0, 4), 2),
               Token(TokenType::Indent, TextPos(0, 6), 2), // TODO Figure out how to end well?
            };
            Subparser parser(tokens);

            auto res = rule.parse(parser);
            REQUIRE(res.get_type() == ParseResult::Type::Success);
            REQUIRE(res.get_consumed() == 3);

            REQUIRE(res.get_nodes().size() == 4);
            REQUIRE(res.get_nodes().at(0).is<IntegerLiteral>());
            REQUIRE(res.get_nodes().at(1).is<Expression>());
            REQUIRE(res.get_nodes().at(2).is<Expression>());
            REQUIRE(res.get_nodes().at(3).is<Expression>());
         }
         SECTION("Deep Fail")
         {

            std::vector<Token> tokens{
               Token(TokenType::Operator, TextPos(0, 0), 2),
               Token(TokenType::Indent, TextPos(0, 2), 2)
            };
            Subparser parser(tokens);

            auto res = rule.parse(parser);
            REQUIRE(res.get_type() == ParseResult::Type::Failure);
            REQUIRE(res.get_consumed() == 2);

            REQUIRE(res.get_nodes().size() == 2);
            REQUIRE(res.get_nodes().at(0).is<Error>());
            REQUIRE(res.get_nodes().at(1).is<Error>());
         }
      }
   }

}