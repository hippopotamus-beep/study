#ifndef BUILD_H
#define BUILD_H

#include <string>
#include <vector>

using namespace std;


class HtmlElement
{
public:
    HtmlElement() = default;
    HtmlElement(const std::string &name, const std::string &text);

    string str(int indent = 0) const;

    string& getName();
    void setName(const string &value);
    __declspec(property(get = getName, put = setName)) string name;

    string& getText();
    void setText(const string &value);
    __declspec(property(get = getText, put = setText)) string text;

    vector<HtmlElement> &getElements();
    void setElements(const vector<HtmlElement> &value);
    __declspec(property(get = getElements, put = setElements)) vector<HtmlElement>
        elements;

private:
    string _name, _text;
    vector<HtmlElement> _elements;
};

class HtmlBuilder
{
public:
    HtmlBuilder(const string &rootName);
    void addChild(const string &childName, const string &childText);
    string str(void);

private:
    HtmlElement root;
};

#endif