#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <iostream>
using namespace std;

#if !defined XMLSAXERRORHANDLER
	#define  XMLSAXERRORHANDLER

class XMLSAXErrorHandler : public HandlerBase
{
	public:
		XMLSAXErrorHandler();
		virtual ~XMLSAXErrorHandler();

    //  Handlers for the SAX ErrorHandler interface
  	virtual void warning(const SAXParseException& exception);
    virtual void error(const SAXParseException& exception);
    virtual void fatalError(const SAXParseException& exception);
};

// Empty default constructor
XMLSAXErrorHandler::XMLSAXErrorHandler()
{
}

// Empty default destructor
XMLSAXErrorHandler::~XMLSAXErrorHandler()
{
}

//  Overrides of the SAX ErrorHandler interface
void XMLSAXErrorHandler::error(const SAXParseException& e)
{
	std::cerr << endl << "Error at file " << XMLString::transcode(e.getSystemId())
				<< ", line " << e.getLineNumber()
		 		<< ", char " << e.getColumnNumber() << endl
				<< "  Message: " << XMLString::transcode(e.getMessage()) << endl;
}

void XMLSAXErrorHandler::fatalError(const SAXParseException& e)
{
  std::cerr	<< "\nFatal Error at file " << XMLString::transcode(e.getSystemId())
  			<< ", line " << e.getLineNumber()
			<< ", char " << e.getColumnNumber()
			<< "\n  Message: " << XMLString::transcode(e.getMessage()) << endl;
}

void XMLSAXErrorHandler::warning(const SAXParseException& e)
{
	std::cerr << "\nWarning at file " << XMLString::transcode(e.getSystemId())
				<< ", line " << e.getLineNumber()
		 		<< ", char " << e.getColumnNumber()
				<< "\n  Message: " << XMLString::transcode(e.getMessage()) << endl;
}
#endif // XMLSAXERRORHANDLER

