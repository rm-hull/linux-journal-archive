// XML Parsing includes
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/SAXParser.hpp>

#include <iostream>
#include <string>

#if !defined XMLSAXPARSER
	#define  XMLSAXPARSER

#include "XMLParserInterface.h"

class XMLNode;

// T - Document Handler
// U - ErrorHandler
template<typename T, typename U>
class XMLSAXParser : public XMLParserInterface
{
	public:
    XMLSAXParser();
    virtual ~XMLSAXParser();

    virtual bool parse();
    virtual bool parse(const std::string&);

    T& getDocumentHandler() { return docHandler; }
	XMLNode& getRootElement() { return docHandler.getRootElement(); }
	
	virtual void doValidation(bool v = true) { validate = v; }
	virtual void doSchema(bool s = true) { schema = s; }
	virtual void fullSchemaChecking(bool s = true) { schemaChecking = s; }
	virtual void namespaces(bool n = true) { names = n; }
	
 private:
	SAXParser* 	parser;
	std::string	xmlFile;

	T		docHandler;
	U		errorHandler;

	bool validate;
	bool schema;
	bool schemaChecking;
	bool names;
	
	bool getValidation() { return validate; }
	bool getSchema() { return schema; }
	bool getSchemaChecking() { return schemaChecking; }
	bool getNamespaces() { return names; }
	
	void init();
	void deinit();
};

template<typename T, typename U>
XMLSAXParser<T, U>::XMLSAXParser()
{
	parser = NULL;
	this->init();
}

template<typename T, typename U>
XMLSAXParser<T, U>::~XMLSAXParser()
{
	this->deinit();
}

template<typename T, typename U>
void XMLSAXParser<T, U>::init()
{
	this->doValidation();
	this->doSchema();
	this->fullSchemaChecking();
	this->namespaces();
	
	try
	{
		XMLPlatformUtils::Initialize();
	}
	catch (const XMLException& toCatch)
	{
		cerr << "Error during initialization! Message:\n"
			 << XMLString::transcode(toCatch.getMessage()) << endl;
	}
}

template<typename T, typename U>
void XMLSAXParser<T, U>::deinit()
{
	//  Delete the parser
	if(parser)
		delete parser;

	// And call the termination method
	XMLPlatformUtils::Terminate();
}

template<typename T, typename U>
bool XMLSAXParser<T, U>::parse()
{
	return this->parse(xmlFile);
}

template<typename T, typename U>
bool XMLSAXParser<T, U>::parse(const std::string& file)
{
	xmlFile = file;
	bool errorOccurred = false;

	//  Create a SAX parser object. Then, according to what we were told on
	//  the command line, set it to validate or not.
	parser = new SAXParser;
	if(!parser)
		return false;

	parser->setDoValidation(this->getValidation());
	parser->setDoNamespaces(this->getNamespaces());
	parser->setDoSchema(this->getSchema());
	parser->setValidationSchemaFullChecking(this->getSchemaChecking());

	parser->setDocumentHandler(&docHandler);
	parser->setErrorHandler(&errorHandler);

	try
	{
		parser->parse(file.c_str());
	}
	catch (const XMLException& e)
	{
		cerr << "\nError during parsing: '" << file << "'\n"
			<< "Exception message is:  \n"
			<< XMLString::transcode(e.getMessage()) << "\n" << endl;
		errorOccurred = true;
	}
	catch (...)
	{
		cerr << "\nUnexpected exception during parsing: '" << file << "'\n";
		errorOccurred = true;
	}

	return (!errorOccurred);
}

#endif // XMLSAXPARSER
