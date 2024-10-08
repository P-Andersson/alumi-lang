#include <catch2/catch_test_macros.hpp>

#include <utf8cpp/utf8.h>

#include <string>
#include <tuple>

import alccemy.util.tuple;

using namespace alccemy;

TEST_CASE("Test Tuple For Each") {
   SECTION("Basic") {
      std::tuple<std::string, int, bool> tuple("test", 42, false);

      int iterations = 0;
      tuple_for_each(tuple, [&](const auto& val) {
         using ValT = std::decay_t<decltype(val)>;
         iterations += 1;
         if constexpr (std::same_as<std::string, ValT>) {
            REQUIRE(val == "test");
         } else if constexpr (std::same_as<int, ValT>) {
            REQUIRE(val == 42);
         } else if constexpr (std::same_as<bool, ValT>) {
            REQUIRE(val == false);
         }
      });

      REQUIRE(iterations == 3);
   }

   SECTION("Indicies") {
      std::tuple<std::string, int, bool> tuple("test", 42, false);

      int iterations = 0;
      tuple_for_each(tuple, [&](const auto& val, size_t index) {
         using ValT = std::decay_t<decltype(val)>;
         iterations += 1;
         if constexpr (std::same_as<std::string, ValT>) {
            REQUIRE(val == "test");
            REQUIRE(index == 0);
         } else if constexpr (std::same_as<int, ValT>) {
            REQUIRE(val == 42);
            REQUIRE(index == 1);
         } else if constexpr (std::same_as<bool, ValT>) {
            REQUIRE(val == false);
            REQUIRE(index == 2);
         }
      });

      REQUIRE(iterations == 3);
   }

   SECTION("Mutations") {
      std::tuple<std::string, int, bool> tuple("test", 42, false);

      int iterations = 0;
      tuple_for_each(tuple, [&](auto& val) {
         using ValT = std::decay_t<decltype(val)>;
         iterations += 1;
         if constexpr (std::same_as<std::string, ValT>) {
            val += "_mutated";
         } else if constexpr (std::same_as<int, ValT>) {
            val += 2;
         } else if constexpr (std::same_as<bool, ValT>) {
            val = !val;
         }
      });

      REQUIRE(iterations == 3);

      REQUIRE(std::get<0>(tuple) == "test_mutated");
      REQUIRE(std::get<1>(tuple) == 44);
      REQUIRE(std::get<2>(tuple) == true);
   }
}

TEST_CASE("Test Tuple For") {
   SECTION("Two") {
      tuple_for(std::make_tuple(1, 2), []<size_t... indicies>(const std::index_sequence<indicies...>) {
         auto index_size = (sizeof...(indicies));
         REQUIRE(index_size == 2);
      });
   }
   SECTION("Zero") {
      tuple_for(std::make_tuple(), []<size_t... indicies>(const std::index_sequence<indicies...>) {
         auto index_size = (sizeof...(indicies));
         REQUIRE(index_size == 0);
      });
   }
   SECTION("Five with return value") {
      auto res =
          tuple_for(std::make_tuple(1, 2, 3, 4, "yo"), []<size_t... indicies>(const std::index_sequence<indicies...>) {
             auto index_size = (sizeof...(indicies));
             REQUIRE(index_size == 5);
             return std::string("yes");
          });
      REQUIRE(res == "yes");
   }
}
