#include <iostream>

#include "chapter2/factory.h"

int main() {
  auto basic = Wall::factory.create_wall({0, 0}, {0, 3000}, 2700, 3000);
  cout << *basic << endl;
  return 0;
}