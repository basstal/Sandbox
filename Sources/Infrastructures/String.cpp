#include "String.hpp"

#include <sstream>
#include <string>
#include <vector>

std::vector<std::string> String::Split(const std::string& source, const char& seperator)
{
	std::vector<std::string> reuslt;
	std::istringstream istringstream(source);
	std::string token;

	while (std::getline(istringstream, token, seperator))
	{
		reuslt.emplace_back(token);
	}
	return reuslt;
}


std::string String::Replace(const std::string& source, const std::string& from, const std::string& to)
{
	std::string result = source;
	size_t start_pos = 0;
	while ((start_pos = result.find(from, start_pos)) != std::string::npos)
	{
		result.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
	return result;
}
