#include <string>
#include <unordered_map> 
#include <bitset>

void findFirstNonRepeatedCharacter(const std::string &analyzedString)
{
	std::unordered_map<const char, std::bitset<1>> analyzedStringTable;

	// Store all characters of string in a hash table.
	for (auto cit = analyzedString.cbegin(); cit != analyzedString.cend(); ++cit)
	{
		// Character does not exist, insert new value and flag as not yet repeated.
		if (!analyzedStringTable.count(*cit))
		{
			std::bitset<1> bit = std::bitset<1>(std::string("0"));
			std::pair<const char, std::bitset<1>> element(*cit, bit);
			analyzedStringTable.emplace(element);
		}
		// Already exists, flag as repeated character.
		else
		{
			std::bitset<1> bit = std::bitset<1>(std::string("1"));
			analyzedStringTable[*cit] = bit;
		}
	}
	// Search through hash table using original string and return first nonrepeated character.
	for (auto cit = analyzedString.begin(); cit != analyzedString.end(); ++cit)
	{
		if (analyzedStringTable[*cit] == std::bitset<1>(std::string("0")))
		{
			printf("First repeated character: %s", analyzedString[*cit]);
			return;
		}
	}

	printf("String contains no nonrepeated characters");
}

void removeChars(std::string &sourceString, const std::string &removeChars)
{
	// Store characters to remove in a hash table for quick look up.
	std::unordered_map<const char, const char> charactersToRemove;

	for (int i = 0; i < removeChars.size; ++i)
	{
		charactersToRemove.emplace(removeChars[i], removeChars[i]);
	}

	// Remove requested characters from source string.
	int write = 0;
	for (int read = write; read < sourceString.size; ++read)
	{
		// Don't increment the write address if the read character is a character to remove.
		if (charactersToRemove[sourceString[read]] != sourceString[read])
		{
			sourceString[write] = sourceString[read];
			++write;
		}
	}
	sourceString[write] = '\0';
}

void reverseString(std::string &source)
{
	// Reverse whole string.
	reverse(source, source.begin(), source.end() - 1);

	// Reverse words in string.
	for (auto current = source.begin(); current != source.end() - 1; ++current)
	{
		auto begin = current;
		while (current != source.end() - 1 && *current != ' ')
		{
			++current;
		}
		reverse(source, begin, current - 1);
	}
}

void reverse(std::string &source, std::string::iterator begin, std::string::iterator end)
{
	// Swap until begin > end.
	for (; begin < end; ++begin, --end)
	{
		char temp = *begin;
		*begin = *end;
		*end = temp;
	}
}