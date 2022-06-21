#include "catch2/catch.hpp"

#include "alumi/parser/parse_rule.h"

using namespace alumi;
using namespace alumi::parser;
using namespace alumi::syntax_tree;


namespace {

   std::optional<Node> build_node(const ParseResult& res)
   {
      if (res.get_type() == ParseResult::Type::Failure)
      {
         return Node(Error(res.get_nodes()), res);
      }
      return Node(IntegerLiteral(), res);
   };

   std::optional<Node> build_node2(const ParseResult& res)
   {
      if (res.get_type() == ParseResult::Type::Failure)
      {
         return Node(Error(res.get_nodes()), res);
      }
      return Node(Expression(), res);
   };
   std::optional<Node> build_node3(const ParseResult& res)
   {
      if (res.get_type() == ParseResult::Type::Failure)
      {
         return Node(Error(res.get_nodes()), res);
      }
      return Node(FunctionCall(), res);
   };


   TEST_CASE("Test Parse Rule - Simple Sequence")
   {
      SECTION("Simple Sequence")
      {
         auto rule = ParseRule<Sequence<Is<TokenType::Indent>, Is<TokenType::Literal>>, NeverSynchroize, build_node>();

         SECTION("Ok")
         {

            std::vector<Token> tokens{
               Token(TokenType::Indent, TextPos(0, 0, 0), 2),
               Token(TokenType::Literal, TextPos(0, 2, 2), 2),
               Token(TokenType::EndOfFile, TextPos(0, 4, 4), 0)
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
               Token(TokenType::Indent, TextPos(0, 0, 0), 2),
               Token(TokenType::Indent, TextPos(0, 2, 2), 2),
               Token(TokenType::EndOfFile, TextPos(0, 4, 4), 0)
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

   class Rule3 : public ParseRule < Sequence<Is<TokenType::Operator>, Is<TokenType::Operator>>, NeverSynchroize, build_node3> {};
   class Rule2 : public ParseRule < Sequence< Is<TokenType::Symbol>, Optional<Rule2>>, NeverSynchroize, build_node2> {};
   class Branch : public AnyOf<Rule2, Rule3>{};
   class Rule1 : public ParseRule<Sequence< Branch>, NeverSynchroize, build_node> {};

   TEST_CASE("Test Parse Rule - Complex Test")
   {
      SECTION("Sequene followed by Branch")
      {
         auto rule = Rule1();

         SECTION("Ok - Branch 1")
         {

            std::vector<Token> tokens{
               Token(TokenType::Operator, TextPos(0, 0, 0), 2),
               Token(TokenType::Operator, TextPos(0, 2, 2), 2),
               Token(TokenType::EndOfFile, TextPos(0, 4, 4), 0)
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
               Token(TokenType::Symbol, TextPos(0, 0, 0), 2),
               Token(TokenType::Symbol, TextPos(0, 2, 2), 2),
               Token(TokenType::Symbol, TextPos(0, 4, 4), 2),
               Token(TokenType::EndOfFile, TextPos(0, 8, 8), 0)
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
               Token(TokenType::Operator, TextPos(0, 0, 0), 2),
               Token(TokenType::Indent, TextPos(0, 2, 2), 2),
               Token(TokenType::EndOfFile, TextPos(0, 4, 4), 0)
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

   TEST_CASE("Test Parse Rule - Fail, Panic, Synch")
   {
      class Rule3 : public ParseRule < Sequence<Is<TokenType::SubscopeBegin>, Is<TokenType::Symbol>, Is<TokenType::SubScopeEnd>>,
         SynchronzieOnMatchedPair<TokenType::SubscopeBegin, TokenType::SubScopeEnd>,
         build_node3> {};
      class Rule2 : public ParseRule<Sequence< Repeats<Is<TokenType::Symbol>>, Is<TokenType::Indent>>, SynchronizeOnToken<TokenType::Indent>, build_node2> {};
      class Rule1 : public ParseRule<Sequence<AnyOf<Rule2, Rule3>, Is<TokenType::EndOfFile>>, NeverSynchroize, build_node> {};

      SECTION("SynchOnToken")
      {
         std::vector<Token> tokens{
            Token(TokenType::Symbol, TextPos(0, 0, 0), 2),
            Token(TokenType::Symbol, TextPos(0, 2, 2), 2),
            Token(TokenType::Operator, TextPos(0, 4, 4), 2),
            Token(TokenType::Indent, TextPos(0, 6, 6), 2),
            Token(TokenType::EndOfFile, TextPos(0, 8, 8), 0)
         };
         Subparser parser(tokens);

         Rule1 rule;
         auto res = rule.parse(parser);

         REQUIRE(res.get_type() == ParseResult::Type::RecoveredFailure);
         REQUIRE(res.get_consumed() == 5);

         REQUIRE(res.get_nodes().size() == 2);
         REQUIRE(res.get_nodes().at(0).is<IntegerLiteral>());
         REQUIRE(res.get_nodes().at(1).is<Error>());
      }
      SECTION("SynchronzieOnMatchedPair")
      {
         std::vector<Token> tokens{
            Token(TokenType::SubscopeBegin, TextPos(0, 0, 0), 2),
            Token(TokenType::SubscopeBegin, TextPos(0, 2, 2), 2),
            Token(TokenType::SubScopeEnd, TextPos(0, 4, 4), 2),
            Token(TokenType::SubScopeEnd, TextPos(0, 6, 6), 2),
            Token(TokenType::EndOfFile, TextPos(0, 8, 8), 0)
         };
         Subparser parser(tokens);

         Rule1 rule;
         auto res = rule.parse(parser);

         REQUIRE(res.get_type() == ParseResult::Type::RecoveredFailure);
         REQUIRE(res.get_consumed() == 5);

         REQUIRE(res.get_nodes().size() == 2);
         REQUIRE(res.get_nodes().at(0).is<IntegerLiteral>());
         REQUIRE(res.get_nodes().at(1).is<Error>());
      }
      SECTION("FailSynchronzieOnMatchedPair - Mismatched Pairs")
      {
         std::vector<Token> tokens{
            Token(TokenType::SubscopeBegin, TextPos(0, 0, 0), 2),
            Token(TokenType::SubscopeBegin, TextPos(0, 2, 2), 2),
            Token(TokenType::SubScopeEnd, TextPos(0, 4, 4), 2),
            Token(TokenType::EndOfFile, TextPos(0, 8, 8), 0)
         };
         Subparser parser(tokens);

         Rule1 rule;
         auto res = rule.parse(parser);

         REQUIRE(res.get_type() == ParseResult::Type::Failure);
         REQUIRE(res.get_consumed() == 4);

         REQUIRE(res.get_nodes().size() == 1);
         REQUIRE(res.get_nodes().at(0).is<Error>());
      }

   }
   
}