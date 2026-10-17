#include "build.h"

#include <iostream>

// class HtmlElement
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

unique_ptr<HtmlBuilder> HtmlElement::create(const string &rootName)
{
    return make_unique<HtmlBuilder>(rootName);
}


// class HtmlBuilder
HtmlBuilder::HtmlBuilder(const string &rootName)
{
    root.name = rootName;
}

HtmlBuilder&  HtmlBuilder::addChild(const string &childName, const string &childText)
{
    HtmlElement e{childName, childText};
    root.elements.emplace_back(e);
    return *this;
}

string HtmlBuilder::str(void){
    return root.str();
}

HtmlBuilder::operator HtmlElement() const { return root; }

// class Tag
// friend function
ostream &operator <<(ostream &os, const Tag &tag){
    os << "<" << tag._name << ">\n";
    for (const auto &a : tag._attributes){
        os << " " << a.first << "=\"" << a.second << "\"\n";
    }
    for (const auto &c : tag._children){
        os << c;
    }
    os << "</" << tag._name << ">\n";
    return os;
}

// public function
vector<pair<string, string>> &Tag::getAttributes() { return _attributes; }
void Tag::setAttributes(const vector<pair<string, string>> &value) { _attributes = value; }

// protected constructor
Tag::Tag(const string &name, const string &text) : _name(name), _text(text) {}
Tag::Tag(const string &name, const vector<Tag> &children) : _name(name), _children(children) {}   

// class P
P::P(const string &text) : Tag("p", text) {}
P::P(initializer_list<Tag> children) : Tag("p", children) {}

// class IMG
IMG::IMG(const string &url) : Tag("img", "") {
    attributes.emplace_back("src", url);
}

// class Person
// friend function
ostream &operator <<(ostream &os, const Person &person){
    os << "street_address: " << person._street_address
       << " post_code: " << person._post_code
       << " city: " << person._city
       << " company_name: " << person._company_name
       << " position: " << person._position
       << " annual_income: " << person._annual_income;
    return os;
}

// public
PersonBuilder Person::create(){
    return PersonBuilder();
}

// private
string& Person::getStreet_address(){
    return _street_address;
}

void Person::setStreet_address(const string &street_address){
    _street_address = street_address;
}

string& Person::getPost_code(){
    return _post_code;
}
  
void Person::setPost_code(const string &post_code){
    _post_code = post_code;
}

string& Person::getCity(){
    return _city;
}

void Person::setCity(const string &city){
    _city = city;
}

string& Person::getCompany_name(){
    return _company_name;
}

void Person::setCompany_name(const string& company_name){
    _company_name = company_name;
}
string& Person::getPosition(){
    return _position;
}

void Person::setPosition(const string& position){
    _position = position;
}

int Person::getAnnual_income(){
    return _annual_income;
}

void Person::setAnnual_income(int annual_income){
    _annual_income = annual_income;
}

// class PersonBuilderBase
//public
PersonBuilderBase::operator Person(){ return std::move(person);}

PersonAddressBuilder PersonBuilderBase::lives() const{
    return PersonAddressBuilder{_person};
}

PersonJobBuilder PersonBuilderBase::works() const{
    return PersonJobBuilder{_person};
}

// protected
PersonBuilderBase::PersonBuilderBase(Person& person) : _person{person} {}

Person& PersonBuilderBase::getPerson(){
    return _person;
}

void PersonBuilderBase::setPerson(const Person &person){
    _person = person;
}

// class PersonAddressBuilder
// public
PersonAddressBuilder::PersonAddressBuilder(Person &person) : PersonBuilderBase(person){}

PersonAddressBuilder::self& PersonAddressBuilder::at(string street_address){
    person.street_address = street_address;
    return *this;
}

PersonAddressBuilder::self& PersonAddressBuilder::with_postcode(string post_code){
    person.post_code = post_code;
    return *this;
}

PersonAddressBuilder::self& PersonAddressBuilder::in(string city){
    person.city = city;
    return *this;
}

// class PersonJobBuilder
PersonJobBuilder::PersonJobBuilder(Person &person) : PersonBuilderBase(person){}

PersonJobBuilder::self& PersonJobBuilder::at(string company_name){
    person.company_name = company_name;
    return *this;
}

PersonJobBuilder::self& PersonJobBuilder::as_a(string position){
    person.position = position;
    return *this;
}

PersonJobBuilder::self& PersonJobBuilder::earning(int annual_income){
    person.annual_income = annual_income;
    return *this;
}

// class MailService
// private functions
void MailService::send_email_impl(const Email &email)
{
    cout << "Sending email from: " << email.from << endl;
    cout << "Sending email to: " << email.to << endl;
    cout << "Sending email subject: " << email.subject << endl;
    cout << "Sending email body: " << email.body << endl;
}