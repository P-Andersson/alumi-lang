#include <catch2/catch_test_macros.hpp>

#include <string>
#include <tuple>
#include <type_traits>
#include <variant>

import alccemy.util.unique_type_args;

using namespace alccemy;

TEST_CASE("Test WithUniqueTypeArgs") {
   SECTION("No-duplicate Types") {
      WithUniqueTypeArgs<std::variant, int, bool, std::string> variant(2);

      REQUIRE(std::is_same_v<decltype(variant), std::variant<int, bool, std::string>>);
   }

   SECTION("Duplicate Types") {
      struct MyType {};
      WithUniqueTypeArgs<std::variant, MyType, int, bool, int, std::string, MyType> variant(2);

      REQUIRE(std::is_same_v<decltype(variant), std::variant<MyType, int, bool, std::string>>);
   }

   SECTION("Strip Void Types") {
      WithUniqueTypeArgs<std::variant, int, void, bool, void, std::string> variant(2);

      REQUIRE(std::is_same_v<decltype(variant), std::variant<int, bool, std::string>>);
   }

   SECTION("No Types") {
      WithUniqueTypeArgs<std::tuple> tuple;

      REQUIRE(std::is_same_v<decltype(tuple), std::tuple<>>);
   }
}
