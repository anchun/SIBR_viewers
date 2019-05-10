#ifndef __SIBR_SYSTEM_XMLTREE_H__
#define __SIBR_SYSTEM_XMLTREE_H__


#include "rapidxml-1.13/rapidxml.hpp"
#include <string>
#include "Config.hpp"

/*
Wrapper of rapidxml xml_document<> class so that the string associated to the xml file stays in memory.
Needed to access nodes by their names
*/

namespace sibr {

	class SIBR_SYSTEM_EXPORT XMLTree : public rapidxml::xml_document<>
	{
	public:
		XMLTree(const std::string & path);

		~XMLTree(void);

	private:
		std::string xmlString;
	};
}

#endif // XMLTREE_H