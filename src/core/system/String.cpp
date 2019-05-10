
#include "core/system/String.hpp"

namespace sibr
{
	std::string strSearchAndReplace( const std::string& src, const std::string& search, const std::string& replaceby )
	{
		size_t pos = src.find(search);
		if (pos != std::string::npos)
		{
			std::string out;
			out = src.substr(0, pos) + replaceby + src.substr(pos+search.size(), src.size()-pos+search.size());
			return out;
		}
		return src;
	}

	bool strContainsOnlyDigits(const std::string& str)
	{
		for (char c : str)
			if (c < '0' || c > '9')
				return false;
		return true;
	}

} // namespace sirb

