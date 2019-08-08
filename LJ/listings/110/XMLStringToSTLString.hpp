#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <string>

#if !defined XMLSTRTOSTLSTR
		#define  XMLSTRTOSTLSTR

class XMLStringToSTLString
{
public:
	XMLStringToSTLString(const XMLCh* const toTranscode)
	{
		str = XMLString::transcode(toTranscode);
	}

	~XMLStringToSTLString() {}

	const std::string& data() const {
		return str;
	}

private:
	std::string	str;
};

inline std::ostream& operator<<(std::ostream& target, const XMLStringToSTLString& toDump)
{
    target << toDump.data();
    return target;
}
#endif // XMLSTRTOSTLSTR
