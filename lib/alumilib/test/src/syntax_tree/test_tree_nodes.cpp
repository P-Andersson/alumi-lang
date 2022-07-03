#include "catch2/catch.hpp"

#include "alumi/syntax_tree/tree_nodes.h"
#include "alumi/syntax_tree/nodes.h"

#include <typeindex>

using namespace alumi;
using namespace alumi::syntax_tree;

namespace {

   bool operator==(const ChildGroup& l, const ChildGroup& r)
   {
      return (l.get_skips() == r.get_skips()) && (l.get_count() == r.get_count());
   }


   class GatherTypesOp
   {
   public:
      std::vector<std::type_index> indices;

      void operator()(SyntaxTreeWalker& walker, Node& node)
      {
         node.visit([&](auto& specific_node) {
            indices.push_back(typeid(specific_node));
         });

         for (size_t i = 0; i < node.child_group_count(); ++i)
         {
            walker.walk(*this, node.child_group(i));
         }
      }
   };

   class GatherTokenStartOp
   {
   public:
      std::vector<size_t> starts;

      void operator()(SyntaxTreeWalker& walker, Node& node)
      {
         starts.push_back(std::get<0>(node.spans_tokens()));
     
         for (size_t i = 0; i < node.child_group_count(); ++i)
         {
            walker.walk(*this, node.child_group(i));
         }
      }
   };


   TEST_CASE("Node Children")
   {
      SECTION("Child Group")
      {
         ChildGroup group(1, 4);

         REQUIRE(group.get_skips() == 1);
         REQUIRE(group.get_count() == 4);
      }

      SECTION("Child Groups")
      {
         ChildGroups<3> groups({ ChildGroup(0, 2), ChildGroup(2, 1), ChildGroup(3, 2) });

         REQUIRE(groups.recursive_subnode_count() == 5);
         REQUIRE(groups.get_group(0) == ChildGroup(0, 2));
         REQUIRE(groups.get_group(1) == ChildGroup(2, 1));
         REQUIRE(groups.get_group(2) == ChildGroup(3, 2));
      }
   }
   TEST_CASE("Syntax Tree Walker")
   {
      SECTION("Basic Walk")
      {
         Nodes nodes;

         nodes.insert(nodes.begin(), Node(Expression(), 3, 4));
         nodes.insert(nodes.begin(), Node(Expression(), 2, 3));
         nodes.insert(nodes.begin(), Node(Statement({}), 1, 2));
         nodes.insert(nodes.begin(), Node(ModuleRoot(nodes), 0, 1));
 
         
         SyntaxTree tree(parser::ParseResult(parser::ParseResult::Type::Success, parser::Subparser({}), nodes), nullptr);


         SyntaxTreeWalker walker(tree);

         GatherTypesOp type_gatherer;
         walker.walk_from_root(type_gatherer);

         REQUIRE(type_gatherer.indices == std::vector<std::type_index>{
            typeid(ModuleRoot),
            typeid(Statement),
            typeid(Expression),
            typeid(Expression),
         });
      }
      SECTION("Walk With Children")
      {
         Nodes children1 = { Node(Statement({}), 1, 2), Node(Statement({}), 2, 3) };
         Nodes children3 = { Node(Statement({}), 6, 7), Node(Statement({}), 7, 8) };
         Nodes children2 = { Node(Statement({}), 4, 5), Node(Statement({children3}), 5, 8) };
         children2.insert(children2.end(), children3.begin(), children3.end());
         Nodes nodes = { Node(Statement({children1}), 0, 3) };
         nodes.insert(nodes.end(), children1.begin(), children1.end());
         nodes.push_back(Node(Statement({children2}), 3, 8));
         nodes.insert(nodes.end(), children2.begin(), children2.end());

         SyntaxTree tree(parser::ParseResult(parser::ParseResult::Type::Success, parser::Subparser({}), nodes), nullptr);


         SyntaxTreeWalker walker(tree);

         GatherTokenStartOp start_gatherer;
         walker.walk_from_root(start_gatherer);

         REQUIRE(start_gatherer.starts == std::vector<size_t>{0, 1, 2, 3, 4, 5, 6, 7});
      }

   }
}