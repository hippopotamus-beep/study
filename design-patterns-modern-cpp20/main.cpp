#include <iostream>

#include "chapter2/factory.h"


int main() {
  const auto main_wall = SolidWall::create_main({0, 0}, {0, 3000}, 2700, 3000);
  cout << main_wall << endl;

  return 0;
}