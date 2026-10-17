#ifndef BUILD_H
#define BUILD_H

#include <memory>
#include <string>
#include <vector>


using namespace std;

class HtmlBuilder;

class HtmlElement {
public:
  friend class HtmlBuilder;

  string str(int indent = 0) const;

  string &getName();
  void setName(const string &value);
  __declspec(property(get = getName, put = setName)) string name;

  string &getText();
  void setText(const string &value);
  __declspec(property(get = getText, put = setText)) string text;

  vector<HtmlElement> &getElements();
  void setElements(const vector<HtmlElement> &value);
  __declspec(property(get = getElements, put = setElements)) vector<HtmlElement>
      elements;

  static unique_ptr<HtmlBuilder> create(const string &rootName);

protected:
  HtmlElement() {}
  HtmlElement(const string &name, const string &text)
      : _name{name}, _text{text} {}

private:
  string _name, _text;
  vector<HtmlElement> _elements;
};

class HtmlBuilder {
public:
  HtmlBuilder(const string &rootName);

  HtmlBuilder &addChild(const string &childName, const string &childText);
  string str(void);
  operator HtmlElement() const;

private:
  HtmlElement root;
};

class Tag {
public:
  friend ostream &operator<<(ostream &os, const Tag &tag);

protected:
Tag(const string &name, const string &text);
Tag(const string &name, const vector<Tag> &children);

  vector<pair<string, string>> &getAttributes();
  void setAttributes(const vector<pair<string, string>> &value);
  __declspec(property(get = getAttributes, put = setAttributes)) vector<pair<string, string>> attributes;

private : 
  string _name;
  string _text;
  vector<Tag> _children;
  vector<pair<string, string>> _attributes;
  
};

class P : public Tag {
public:
  explicit P(const string &text);
  P(initializer_list<Tag> children);
};

class IMG : public Tag {
public:
  explicit IMG(const string &url);
};

class PersonBuilder;
class Person{
  friend class PersonAddressBuilder;
  friend class PersonJobBuilder;
  friend class PersonBuilder;
public:
  friend ostream &operator<<(ostream &os, const Person &person);

  static PersonBuilder create();
private:
  Person(){}

  string& getStreet_address();
  void setStreet_address(const string &street_address);
  __declspec(property(get = getStreet_address, put = setStreet_address)) string street_address;
  string& getPost_code();
  void setPost_code(const string &post_code);
  __declspec(property(get = getPost_code, put = setPost_code)) string post_code;
  string& getCity();
  void setCity(const string &city);
  __declspec(property(get = getCity, put = setCity)) string city;

  string& getCompany_name();
  void setCompany_name(const string& company_name);
  __declspec(property(get = getCompany_name, put = setCompany_name)) string company_name;
  string& getPosition();
  void setPosition(const string& position);
  __declspec(property(get = getPosition, put = setPosition)) string position;
  int getAnnual_income();
  void setAnnual_income(int annual_income);
  __declspec(property(get = getAnnual_income, put = setAnnual_income)) int annual_income;

  string _street_address, _post_code, _city;
  string _company_name, _position;
  int _annual_income = 0;
};

class PersonAddressBuilder;
class PersonJobBuilder;
class PersonBuilderBase{
public:
  operator Person();

  PersonAddressBuilder lives() const;
  PersonJobBuilder works() const;

protected:
  explicit PersonBuilderBase(Person& person);

  Person& getPerson();
  void setPerson(const Person &person);
  __declspec(property(get = getPerson, put = setPersong)) Person person;
  

private:
  Person& _person;
};

class PersonBuilder : public PersonBuilderBase{
public:
  PersonBuilder() : PersonBuilderBase{p} {}
private:
  Person p;
};

class PersonAddressBuilder : public PersonBuilderBase{
  typedef PersonAddressBuilder self;
public:
  explicit PersonAddressBuilder(Person &person);
  self& at(string street_address);
  self& with_postcode(string post_code);
  self& in(string city);
};

class PersonJobBuilder : public PersonBuilderBase{
  typedef PersonJobBuilder self;
public:
  explicit PersonJobBuilder(Person &person);
  self& at(string company_name);
  self& as_a(string position);
  self& earning(int annual_income);
};


#endif