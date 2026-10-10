#include "build.h"

HtmlElement::HtmlElement(const std::string &name, const std::string &text)
    : _name(name), _text(text) {}

string HtmlElement::str(int indent) const
{
    string i(indent, ' ');
    string s = i + "<" + _name + ">" + _text + "\n";

    for (const auto &e : _elements)
    {
        s += e.str(indent + 2);
    }

    s += i + "</" + _name + ">\n";
    return s;
}

string &HtmlElement::getName() { return _name; }
void HtmlElement::setName(const string &value) { _name = value; }

string &HtmlElement::getText() { return _text; }
void HtmlElement::setText(const string &value) { _text = value; }

vector<HtmlElement> &HtmlElement::getElements() { return _elements; }
void HtmlElement::setElements(const vector<HtmlElement> &value) { _elements = value; }

HtmlBuilder::HtmlBuilder(const string &rootName)
{
    root.name = rootName;
}

void HtmlBuilder::addChild(const string &childName, const string &childText)
{
    root.elements.emplace_back(childName, childText);
}

string HtmlBuilder::str(void){
    return root.str();
}