#include <Simple/SimpleClass.hpp>

using namespace Simple;

SimpleClass::SimpleClass()
    : number{0} {}

void SimpleClass::setNumber(int _number) {
  number = _number;
}
int SimpleClass::getNumber() const {
  return number;
}