#ifndef BUILD_H
#define BUILD_H

#include <memory>
#include <string>
#include <vector>
#include <functional>

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


class Email {
public:
  string from, to, subject, body;
};

class EmailBuilder{
public:
  explicit EmailBuilder(Email& email) : email{email} {}
  EmailBuilder& from(string from){
    email.from = from;
    return *this;
  }
  EmailBuilder& to(string to){
    email.to = to;
    return *this;
  }
  EmailBuilder& subject(string subject){
    email.subject = subject;
    return *this;
  }
  EmailBuilder& body(string body){
    email.body = body;
    return *this;
  }
private:
  Email& email;
};

class MailService{
public:
  void send_email(std::function<void(EmailBuilder&)> builder){
    Email email;
    EmailBuilder emailBuilder{email};
    builder(emailBuilder);
    send_email_impl(email);
  }

private:
  void send_email_impl(const Email& email);
};

class Person {
public:
  string name, position, date_of_birth;

  friend ostream &operator<<(ostream &os, const Person &person);
};

class PersonBuilder {
protected:
  Person person;

public:
  [[nodiscard]] Person build() const{ return person;}
};

template <typename TSelf>
class PersonInfoBuilder : public PersonBuilder{
public:
  TSelf& called(const string &name){
    person.name = name;
    return static_cast<TSelf&>(*this);
  }
};

template<typename TSelf>
class PersonJobBuilder : public PersonInfoBuilder<PersonJobBuilder<TSelf>>{
public:
  TSelf& works_as_a(const string &position){
    this->person.position = position;
    return static_cast<TSelf&>(*this);
  }
};

template<typename TSelf>
class PersonBirthDateBuilder : public PersonJobBuilder<PersonBirthDateBuilder<TSelf>>{
public:
  TSelf& born_on(const string &date_of_birth){
    this->person.date_of_birth = date_of_birth;
    return static_cast<TSelf&>(*this);
  }
};

class MyBuilder : public PersonBirthDateBuilder<MyBuilder>{};

#endif