#include <iostream>

#include "chapter1/build.h"

int main(){
    string words[] = {"hello", "world"};
    HtmlElement list{"ul", ""};
    for(auto &w : words){
        list.setElement("li", w);
    }

    cout << list.str() << endl;

    return 0;
}