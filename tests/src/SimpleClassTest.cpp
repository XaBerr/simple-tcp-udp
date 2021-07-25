#include <Simple/SimpleClass.hpp>
#include <catch2/catch.hpp>

using namespace Simple;

TEST_CASE("SimpleClass SimpleClass()", "[simple]") {
  SimpleClass simple;
  REQUIRE(simple.getNumber() == 0);
}

TEST_CASE("SimpleClass setNumber(int _number)", "[simple]") {
  SimpleClass simple;
  simple.setNumber(1);
  REQUIRE(simple.getNumber() == 1);
}