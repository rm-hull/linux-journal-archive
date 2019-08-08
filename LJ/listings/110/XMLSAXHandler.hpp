#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include "XMLStringToSTLString.hpp"

#if !defined XMLSAXHANDLER
	#define  XMLSAXHANDLER

// Forward Reference
class AttributeList;
class XMLNode;

// T - Domain Map
template<typename T>
class XMLSAXHandler : public HandlerBase
{
	typedef T*	root;
public:
	XMLSAXHandler();
	virtual ~XMLSAXHandler();

	T& getDomainMap() { return dataMap; }

	XMLNode& getRootElement() { return dataMap.root(); }
	
	// Handlers for the SAX DocumentHandler interface
	virtual void startElement(const XMLCh* const name, AttributeList& attributes);
	virtual void characters(const XMLCh* const chars, const unsigned int length);
	virtual void ignorableWhitespace(const XMLCh* const chars, const unsigned int length);
	virtual void endElement(const XMLCh* const name);

private:
	// Private data members
	T		dataMap;
};

template<typename T>
XMLSAXHandler<T>::XMLSAXHandler()
{
}

template<typename T>
XMLSAXHandler<T>::~XMLSAXHandler()
{
}

//  SAXParser: Implementation of the SAX DocumentHandler interface
template<typename T>
void XMLSAXHandler<T>::startElement(const XMLCh* const name, AttributeList&  attributes)
{
	XMLStringToSTLString str(name);
	dataMap.create(str.data());
}

template<typename T>
void XMLSAXHandler<T>::endElement(const XMLCh* const name)
{
	XMLStringToSTLString str(name);
	dataMap.add(str.data());
}

template<typename T>
void XMLSAXHandler<T>::characters(const XMLCh* const chars, const unsigned length)
{
	XMLStringToSTLString str(chars);
	dataMap.updateAttribute(str.data());
}

template<typename T>
void XMLSAXHandler<T>::ignorableWhitespace( const XMLCh* const chars, const unsigned int length)
{
}

#endif // XMLSAXHANDLER
