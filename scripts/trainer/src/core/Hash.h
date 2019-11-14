#pragma once
#include <string>

namespace String
{
	template<std::size_t N>
	inline constexpr unsigned int Hash(const char(&str)[N])
	{
		unsigned int value = 0, temp = 0;
		for (size_t i = 0; i < N - 1; i++)
		{
			temp = tolower(str[i]) + value;
			value = temp << 10;
			temp += value;
			value = temp >> 6;
			value ^= temp;
		}
		temp = value << 3;
		temp += value;
		unsigned int temp2 = temp >> 11;
		temp = temp2 ^ temp;
		temp2 = temp << 15;
		value = temp2 + temp;
		return value;
	}

	inline unsigned int Hash(const std::string& str, bool caseSensetive = false)
	{
		unsigned int value = 0, temp = 0;
		for (size_t i = 0; i < str.length(); i++)
		{
			temp = (caseSensetive ? str[i] : ::tolower(str[i])) + value;
			value = temp << 10;
			temp += value;
			value = temp >> 6;
			value ^= temp;
		}
		temp = value << 3;
		temp += value;
		unsigned int temp2 = temp >> 11;
		temp = temp2 ^ temp;
		temp2 = temp << 15;
		value = temp2 + temp;
		return value;
	}
}
