#include <iostream>

#include "chapter1/build.h"

int main(){
    HtmlBuilder builder("ul");
    builder.addChild("li", "hello");
    builder.addChild("li", "world");
    cout << builder.str() << endl;
    
    return 0;
}