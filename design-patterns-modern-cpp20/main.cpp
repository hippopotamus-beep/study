#include <iostream>

#include "chapter1/build.h"

int main() {
  HtmlElement e = HtmlElement::create("ul")
                      ->addChild("li", "hello")
                      .addChild("li", "world");
  cout << e.str() << endl;
  P p{IMG{"http://pokemon.com/pikachu.png"}};
  cout << p << endl;

  Person p1 = Person::create()
                 .lives().at("123 London Road")
                 .in("London").with_postcode("SW12BC")
                 .works().at("Fabrikam").as_a("Engineer")
                 .earning(123000);
  cout << p1 << endl;

  return 0;
}