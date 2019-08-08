#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
using namespace std;

#ifndef XMLNODE
#define XMLNODE

class XMLNode
{
public:
	typedef map<string, string> Attributes;

    XMLNode();
    XMLNode(const XMLNode&);
	XMLNode(const string&);
	XMLNode(const string&, const string&);
    virtual ~XMLNode();

	// Operator overloads
	bool operator==(const XMLNode&);
	bool operator==(const string&);
	bool operator!=(const XMLNode&);
	bool operator!=(const string&);
	bool operator<(const XMLNode&) const;
	bool operator<(const string&) const;
	bool operator>(const XMLNode&) const;
	bool operator>(const string&) const;
    XMLNode& operator=(const XMLNode&);
	
	// Accessors and Mutators
	const string& value() const { return value_; }
	void value(const string& v) { value_ = v; }
	
	const string& name() const { return name_; }
	void name(const string& n) { name_ = n; }

	void addChild(XMLNode&);
	void child(XMLNode&);
	XMLNode child(const string&);
	
	bool hasChild(const string&);
	size_t childCount() { return children_.size(); }
	bool children() { return (!children_.empty()); }

private:
	string			value_;
	string			name_;
	Attributes		attributes_;
	vector<XMLNode>		children_;
};

XMLNode::XMLNode()
{
}

XMLNode::XMLNode(const std::string& n, const std::string& v)
{
	name_ = n;
	value_ = v;
}

XMLNode::XMLNode(const std::string& n)
{
	name_ = n;
}

XMLNode::XMLNode(const XMLNode& rhs)
{
	if(&rhs == this)
		return;

	this->operator=(rhs);
}

XMLNode::~XMLNode()
{
}

XMLNode& XMLNode::operator=(const XMLNode& rhs)
{
	if(&rhs == this)
		return *this;

	value_ = rhs.value_;
	name_ = rhs.name_;
	children_ = rhs.children_;
	return *this;
}

bool XMLNode::operator==(const XMLNode& n)
{
	return ((n.name_ == name_) && (n.value_ == value_));
}

bool XMLNode::operator==(const std::string& n)
{
	return (n == name_);
}

bool XMLNode::operator!=(const XMLNode& n)
{
	return ((n.name_ != name_) && (n.value_ != value_));
}

bool XMLNode::operator!=(const std::string& n)
{
	return (n != name_);
}

bool XMLNode::operator<(const XMLNode& n) const
{
	return ((n.name_ < name_) && (n.value_ < value_));
}

bool XMLNode::operator<(const std::string& n) const
{
	return (strcmp(name_.c_str(), n.c_str()) < 0);
}

bool XMLNode::operator>(const XMLNode& n) const
{
	return ((n.name_ > name_) && (n.value_ > value_));
}

bool XMLNode::operator>(const std::string& n) const
{
	return (strcmp(name_.c_str(), n.c_str()) > 0);
}

void XMLNode::child(XMLNode& c)
{
	children_.push_back(c);
}

bool XMLNode::hasChild(const std::string& name)
{
	vector<XMLNode>::iterator iter = find(children_.begin(), children_.end(), name);
	return (iter != children_.end());
}

XMLNode XMLNode::child(const std::string& name)
{
	vector<XMLNode>::iterator iter = find(children_.begin(), children_.end(), name);
	if(iter != children_.end())
		return *iter;

	return XMLNode();
}

ostream& operator<<(ostream& o, XMLNode& node)
{
	o << "Writing Node name to stream : " << node.name() << endl;
	return o;
}

ostream& operator<<(XMLNode& node, ostream& o)
{
	o << "Writing Node name to stream : " << node.name() << endl;
	return o;
}

void XMLNode::addChild(XMLNode& node)
{
	children_.push_back(node);
}

#endif // XMLNODE
