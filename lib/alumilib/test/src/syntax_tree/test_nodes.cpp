#include "catch2/catch.hpp"

#include "alumi/syntax_tree/nodes.h"

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
         Node node(Error(), TextPos(0, 4), TextPos(3, 12));

         REQUIRE(node.spans_text() == std::make_tuple(TextPos(0, 4), TextPos(3, 12)));
      }

      SECTION("Is")
      {
         Node node(Error(), TextPos(0, 4), TextPos(3, 12));

         REQUIRE(node.is<FunctionCall>() == false);
         REQUIRE(node.is<Error>() == true);
      }

      SECTION("Node Count")
      {
         Node node1(Error(), TextPos(0, 4), TextPos(3, 12));

         REQUIRE(node1.recursive_child_count() == 0);
         
         Nodes nodes({ 
            Node(Declaration(),  TextPos(0, 0), TextPos(0, 2)),
            Node(Expression(),  TextPos(0, 2), TextPos(0, 4)) 
         });
         Node node2(ModuleRoot(nodes), TextPos(0, 0), TextPos(0, 4));

         REQUIRE(node2.recursive_child_count() == 2);
      }

      SECTION("Visit")
      {
         {
            Node node1(Error(), TextPos(0, 4), TextPos(3, 12));

            TestGetTypeOp op;
            REQUIRE(node1.visit(op) == typeid(Error));
         }

         {
            Node node1(Expression(), TextPos(999 , 10), TextPos(1455, 12));

            TestGetTypeOp op;
            REQUIRE(node1.visit(op) == typeid(Expression));
         }
      }
   }
}
