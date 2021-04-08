
#include "hot_reload.h"

#include "foo.h"

#include <iostream>

int main()
{
  FooModule::LoadLibrary();
  std::cout << "foo(1) == " << FooModule::Foo(1) << std::endl;
  std::cout << "bar == " << FooModule::GetBar() << std::endl;

  std::cout << "Make some changes, recompile, and press enter." << std::flush;
  while(std::cin.get() != '\n') {}

  FooModule::ReloadLibrary();
  std::cout << "foo(1) == " << FooModule::Foo(1) << std::endl;
  std::cout << "bar == " << FooModule::GetBar() << std::endl;
  return 0;
}
