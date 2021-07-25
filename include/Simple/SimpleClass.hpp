#ifndef SIMPLE_SIMPLECLASS_H
#define SIMPLE_SIMPLECLASS_H

namespace Simple {
class SimpleClass {
  int number;

 public:
  SimpleClass();
  void setNumber(int _number);
  int getNumber() const;
};
}  // namespace Simple

#endif