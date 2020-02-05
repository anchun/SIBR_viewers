#ifndef __SIBR_SYSTEM_XMLTREE_H__
#define __SIBR_SYSTEM_XMLTREE_H__

#include "rapidxml-1.13/rapidxml.hpp"
#include <string>
#include "Config.hpp"

namespace sibr {

	/**
	Wrapper of rapidxml xml_document<> class so that the string associated to the xml file stays in memory.
	Needed to access nodes by their names.
	* \ingroup sibr_system
	*/
	class SIBR_SYSTEM_EXPORT XMLTree : public rapidxml::xml_document<>
	{
	public:
		/** Construct an XML structure from the content of a file.
		\param path the file path
		*/
		XMLTree(const std::string & path);

		/** Destructor. */
		~XMLTree(void);

		/** Save the XML structure to a file as a string representation.
		\param path output path
		\return a success flag
		*/
		bool save(const std::string & path) const;

	private:
		std::string xmlString; //< Internal copy of the laoded string.
	};
}

#endif // XMLTREE_H