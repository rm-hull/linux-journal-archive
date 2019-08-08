#include <string>

#if !defined XMLPARSERINTERFACE
	#define  XMLPARSERINTERFACE

class XMLParserInterface
{
public:
    virtual bool parse() = 0;
	virtual bool parse(const std::string&) = 0;
	
	virtual void doValidation(bool v = true) = 0;
	virtual void doSchema(bool s = true) = 0;
	virtual void fullSchemaChecking(bool s = true) = 0;
	virtual void namespaces(bool n = true) = 0;
};

#endif // XMLPARSERINTERFACE
