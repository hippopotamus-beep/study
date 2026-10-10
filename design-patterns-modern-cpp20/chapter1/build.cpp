#include "build.h"

// class HtmlElement
HtmlElement::HtmlElement(const std::string &name, const std::string &text)
    : name(name), text(text) {}

string HtmlElement::str(int indent) const
{
    string i(indent, ' ');
    string s = i + "<" + name + ">" + text + "\n";

    for(const auto &e : elements)
    {
        s += e.str(indent + 2);
    }

    s += i + "</" + name + ">\n";
    return s;
}   

void HtmlElement::setElement(const std::string &name, const std::string &text){
    elements.emplace_back(name, text);
}