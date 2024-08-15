#include <catch2/catch_test_macros.hpp>

import alumi.syntax_tree;

#include <typeinfo>
#include <typeindex>

#include <utf8cpp/utf8.h>


using namespace alumi;
using namespace alumi::syntax_tree;

namespace {

   class TestGetTypeOp
   {
   public:
      template <typename T>
      std::type_index operator()(T& t)
      {
         return typeid(t);
      }
   };

   TEST_CASE("Node")
   {
      SECTION("Test Span")
      {
         Node node(Statement({}), 4, 6);

         REQUIRE(node.spans_tokens() == std::make_tuple(4, 6 ));
      }

      SECTION("Is")
      {
         Node node(Statement({}), 4, 6);

         REQUIRE(node.is<FunctionCall>() == false);
         REQUIRE(node.is<Statement>() == true);
      }

      SECTION("Node Count")
      {
         Node node1(Statement({}), 4, 6);

         REQUIRE(node1.recursive_child_count() == 1);
         
         Nodes nodes({ 
            Node(Expression(),  0, 1),
            Node(Expression(),  1, 2) 
         });
         Node node2(ModuleRoot(nodes), 0, 2);

         REQUIRE(node2.recursive_child_count() == 3);
      }

      SECTION("Visit")
      {
         {
            Node node1(Statement({}), 0, 4);

            TestGetTypeOp op;
            REQUIRE(node1.visit(op) == typeid(Statement));
         }

         {
            Node node1(Expression(), 999, 1455);

            TestGetTypeOp op;
            REQUIRE(node1.visit(op) == typeid(Expression));
         }
      }
   }
}
