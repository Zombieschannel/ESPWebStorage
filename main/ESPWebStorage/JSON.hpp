#pragma once
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>

//Class for loading and saving JSON files
class JSON
{
public:
    std::string inClass = "";
	std::map<std::string, std::string> map;
	const std::string& getValue(const std::string& key) const
	{
		return map.at(key);
	}
	bool exists(const std::string& key) const
	{
		return map.find(key) != map.end();
	}
	void getValue(const std::string& key, int& val)
	{
		if (map.find(key) == map.end())
			return;
		val = stoi(map[key]);
	}
	void getValue(const std::string& key, float& val)
	{
		if (map.find(key) == map.end())
			return;
		val = stof(map[key]);
	}
	void loadValue(const std::string& key, std::int32_t& val)
	{
		if (exists(key))
			val = stol(map[key]);
	}
	void loadValue(const std::string& key, std::int64_t& val)
	{
		if (exists(key))
			val = stoll(map[key]);
	}
	void loadValue(const std::string& key, std::int16_t& val)
	{
		if (exists(key))
			val = stol(map[key]);
	}
	void loadValue(const std::string& key, std::int8_t& val)
	{
		if (exists(key))
			val = stol(map[key]);
	}
	void loadValue(const std::string& key, std::uint32_t& val)
	{
		if (exists(key))
			val = stoul(map[key]);
	}
	void loadValue(const std::string& key, std::uint64_t& val)
	{
		if (exists(key))
			val = stoull(map[key]);
	}
	void loadValue(const std::string& key, std::uint16_t& val)
	{
		if (exists(key))
			val = stoul(map[key]);
	}
	void loadValue(const std::string& key, std::uint8_t& val)
	{
		if (exists(key))
			val = stoul(map[key]);
	}
	void loadValue(const std::string& key, float& val)
	{
		if (exists(key))
			val = stof(map[key]);
	}
	void loadValue(const std::string& key, double& val)
	{
		if (exists(key))
			val = stod(map[key]);
	}
	void loadValue(const std::string& key, std::string& val)
	{
		if (exists(key))
			val = map[key];
	}
	void loadValue(const std::string& key, bool& val)
	{
		if (exists(key))
			val = stoi(map[key]);
	}
	template<typename T>
	void setValue(const std::string& key, const T& value)
	{
		map[inClass + key] = std::to_string(value);
	}
	void setClass(const std::string& inClass)
	{
		this->inClass = inClass;
	}
	void setValueStr(const std::string& key, const std::string& value)
	{
		map[inClass + key] = value;
	}
	void loadFromStream(std::ifstream& stream)
	{
		map.clear();
		std::string currentClass = "";
		std::string keyValueName = "";
		bool inQuotes = false;
		std::string brackets = "";
		int arrayIndex = 0;
		char byte = '\0';
		char lastByte = ' ';
		bool commented = false;
		while (1)
		{
            stream.read(&byte, 1);
            if (byte == '\0')
                break;
			if (inQuotes)
			{
				switch (byte)
				{
				case '"':
					inQuotes = !inQuotes;
					break;
				case '\n': case '\r':
					break;
				default:
					keyValueName += byte;
					break;
				}
				continue;
			}
			if (!inQuotes && byte == '/' && lastByte == '/')
				commented = true;
			if (commented)
			{
				if (byte == '\n' || byte == '\r')
					commented = false;
				lastByte = byte;
				continue;
			}
			switch (byte)
			{
			case '{':
				if (brackets.size() > 0)
					currentClass += '.';
				brackets += '{';
				break;
			case '}':
				if (keyValueName.size() > 0)
				{
					map[currentClass] = keyValueName;
					keyValueName = "";
				}
				while (currentClass.size() > 0 && currentClass.back() != '.') currentClass.pop_back();
				if (currentClass.size() > 0)
					currentClass.pop_back();
				if (brackets.back() == '{')
					brackets.pop_back();
				else
					return;
				break;
			case '"':
				inQuotes = !inQuotes;
				break;
			case ',':
				if (brackets.back() == '{')
				{
					map[currentClass] = keyValueName;
					keyValueName = "";
					while (currentClass.size() > 0 && currentClass.back() != '.') currentClass.pop_back();

				}
				else if (brackets.back() == '[')
				{
					if (currentClass.back() == ']')
					{
						map[currentClass] = keyValueName;
						keyValueName = "";
					}
					arrayIndex++;
					currentClass.replace(currentClass.rfind("[" + std::to_string(arrayIndex - 1) + "]"), 2 + std::to_string(arrayIndex - 1).size(), "[" + std::to_string(arrayIndex) + "]");
				}
				break;
			case ':':
				currentClass += keyValueName;
				map[currentClass];
				keyValueName = "";
				break;
			case '[':
				currentClass += "[0]";
				brackets += '[';
				break;
			case ']':
				arrayIndex = 0;
				if (currentClass.back() == ']')
				{
					map[currentClass] = keyValueName;
					keyValueName = "";
				}
				if (brackets.back() == '[')
					brackets.pop_back();
				else
					return;
				break;
			case '\n': case '\r': case ' ': case '\t': case '/':
				break;
			default:
				keyValueName += byte;
				break;
			}
			lastByte = byte;
		}
	}
	bool loadFromFile(const std::string& fileName)
	{
        std::ifstream fis;
        fis.open(fileName);
		if (!fis.is_open())
			return false;
		loadFromStream(fis);
		return true;
	}
	void saveToMemory(std::string& data)
	{
		data += '{';
		std::string currentClass = "";
		std::string brackets = "";
		std::vector<std::string> arrayNamesFound;
		std::string lastClass = "";
		int itemsInClass = 0;
		bool first = true;
		for (auto& n : map)
		{
			if (n.second == "")
				continue;
			std::string entryName = n.first.substr(n.first.rfind('.') + 1);
			std::string inClass = n.first.substr(0, n.first.rfind('.') + 1);

			bool classChanged = false;
			if (currentClass != inClass) // if in wrong class
			{
				std::string subclass = "";
				classChanged = true;
				bool removedData = false;
				//remove
				while (inClass.find(currentClass) != 0)
				{
					currentClass.erase(currentClass.size() - 1);
					int toRemove = currentClass.rfind('.');
					currentClass.erase(toRemove + 1);
					if (brackets.back() == '{')
						data += '}';
					else if (brackets.back() == '[')
						data += ']';
					brackets.pop_back();
					removedData = true;
				}
				//check if out of array
				if (count(lastClass.begin(), lastClass.end(), '[') > count(inClass.begin(), inClass.end(), '[') && brackets.back() == '[')
				{
					data += ']';
					brackets.pop_back();
				}
				if (removedData)
					data += ',';
				//add classes
				if (inClass.find(currentClass) == 0)
				{
					//add mode
					while (1)
					{
						if (currentClass == inClass)
						{
							break;
						}
						else
						{
							subclass = inClass.substr(currentClass.size(), inClass.find('.', currentClass.size()) - currentClass.size());
							bool arrayMode = false;
							bool firstTimeInArray = true;
							std::string arrayModeBetween = "";
							if (subclass.back() == ']')
							{
								int arSize = subclass.rfind(']') - (subclass.rfind('[') + 1);
								arrayModeBetween = subclass.substr(subclass.rfind('[') + 1, arSize);
								subclass.erase(subclass.rfind('['));
								arrayMode = true;
								bool foundArray = false;
								for (auto& n : arrayNamesFound)
									if (n == subclass)
									{
										foundArray = true;
										break;
									}
								if (foundArray)
									firstTimeInArray = false;
								else
									arrayNamesFound.push_back(subclass);
							}
							if (!arrayMode)
							{
								if (!removedData && itemsInClass)
								{
									data += ',';
									itemsInClass = 0;
								}
								data += '"';
								data += subclass;
								data += '"';
								data += ':';
								data += '{';
								brackets.push_back('{');
								currentClass += subclass + ".";
							}
							else
							{
								if (firstTimeInArray)
								{
									if (!removedData && itemsInClass)
									{
										data += ',';
										itemsInClass = 0;
									}
									data += '"';
									data += subclass;
									data += '"';
									data += ':';
									data += '[';
									brackets.push_back('[');
								}
								data += '{';
								brackets.push_back('{');
								currentClass += subclass + "[" + arrayModeBetween + "].";
							}
						}
					}
				}
			}

			if (!classChanged && !first)
				data += ',';
			else
				itemsInClass = 0;
			itemsInClass++;
			data += '"';
			data += entryName;
			data += '"';
			data += ':';
			data += '"';
			data += n.second;
			data += '"';
			lastClass = inClass;
			first = false;
		}
		while (brackets.size() > 0)
		{
			if (brackets.back() == '{')
				data += '}';
			else if (brackets.back() == '[')
				data += ']';
			brackets.pop_back();
		}
		data += '}';
	}
	void saveToFile(const std::string& fileName)
	{
		std::ofstream save;
		save.open(fileName);
		if (save.is_open())
		{
			std::string data = "";
			saveToMemory(data);
			save.write(&data[0], data.size());
			save.close();
		}
	}
};