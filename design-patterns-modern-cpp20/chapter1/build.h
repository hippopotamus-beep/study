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

    void setElement(const std::string &name, const std::string &text);
    
    string str(int indent = 0) const;

private:
    string name, text;
    vector<HtmlElement> elements;
};

#endif