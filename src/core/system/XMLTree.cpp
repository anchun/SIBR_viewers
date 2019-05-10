#include "XMLTree.h"
#include <iostream>
#include <fstream>
#include <sstream>


namespace sibr {
	XMLTree::XMLTree(const std::string &  path)
	{
		std::cout << "Parsing xml file < " << path << " > : ";
		std::ifstream file(path.c_str());
		if (file) {
			std::stringstream buffer;
			buffer << file.rdbuf();
			file.close();
			xmlString = std::move(std::string(buffer.str()));
			this->parse<0>(&xmlString[0]);
			std::cout << "succes " << std::endl;
		}
		else {
			std::cout << "error, cant open file " << std::endl;
		}
	}


	XMLTree::~XMLTree(void)
	{
	}

}
