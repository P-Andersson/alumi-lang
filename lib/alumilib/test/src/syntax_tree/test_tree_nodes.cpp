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

      void operator()(ModuleTreeWalker& walker, Node& node)
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


   TEST_CASE("Tree Nodes")
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

      SECTION("Module Tree Walker")
      {
         Nodes nodes;

         nodes.insert(nodes.begin(), Node(Expression(), 3, 4));
         nodes.insert(nodes.begin(), Node(Expression(), 2, 3));
         nodes.insert(nodes.begin(), Node(Statement(), 1, 2));
         nodes.insert(nodes.begin(), Node(ModuleRoot(nodes), 0, 1));
 
         
         ModuleTree tree(nodes);


         ModuleTreeWalker walker(tree);

         GatherTypesOp type_gatherer;
         walker.walk_from_root(type_gatherer);

         REQUIRE(type_gatherer.indices == std::vector<std::type_index>{
            typeid(ModuleRoot),
            typeid(Statement),
            typeid(Expression),
            typeid(Expression),
         });

      }
   }
}