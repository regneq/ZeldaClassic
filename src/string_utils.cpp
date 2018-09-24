#include "string_utils.h"

#include <cctype>
#include <sstream>

using namespace std;

string zc::to_upper(string const& str)
{
	ostringstream oss;
	for (string::const_iterator it = str.begin(); it != str.end(); ++it)
		oss << char(toupper(*it));
	return oss.str();
}

string zc::to_lower(string const& str)
{
	ostringstream oss;
	for (string::const_iterator it = str.begin(); it != str.end(); ++it)
		oss << char(tolower(*it));
	return oss.str();
}

