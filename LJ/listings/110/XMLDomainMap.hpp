// STL related includes
#include <stack>
#include <list>
#include <unistd.h>
using namespace std;

#ifndef XMLDATAMAP
#define XMLDATAMAP

template<typename T>
class XMLDomainMap
{
	typedef T				DomainObject;
	typedef std::stack<DomainObject>	DomainObjectStack;
	typedef std::list<DomainObject>         DomainObjectList;


public:
	XMLDomainMap() {
	}
	
	XMLDomainMap(const XMLDomainMap<T>& rhs) {
		if(&rhs == this)
			return;

		this->operator=(rhs);
		return;
	}
	
	virtual ~XMLDomainMap() {
	}

	XMLDomainMap<T>& operator=(const XMLDomainMap<T>& rhs) {
		if(&rhs == this)
			return *this;

		objectStack = rhs.objectStack;
		objectList = rhs.objectList;
		return *this;
	}

	// public interface
	void create(const std::string& name) {
		T obj(name);
		objectStack.push(obj);
	}

	// called from the endElement method of SAX Handler
	void add(const std::string& name) {
		T child = objectStack.top();
		child.name(name);
		objectStack.pop();

		if(objectStack.size())
		{
			T parent = objectStack.top();
			objectStack.pop();
			cout << "Adding child " << child.name() << endl;
			parent.addChild(child);
			objectStack.push(parent);
		}
		else
		{
			objectList.push_back(child);
		}
	}

	// called from the characters method of SAX Handler
	void updateAttribute(const std::string& v) {
		T obj = objectStack.top();
		objectStack.pop();
		obj.value(v);
		objectStack.push(obj);
	}

	size_t size() { return objectList.size(); }
	DomainObject& root() { 
		DomainObjectList::iterator start = objectList.begin();
		return *start; 
	}

private:
	DomainObjectStack	objectStack;
	DomainObjectList	objectList;
};
#endif // XMLDATAMAP
