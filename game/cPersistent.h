#pragma once

#include <string>
#include <unordered_map>

#include "cTools.h"

class cPersistent
{
	struct Data
	{
		std::string name;

		std::string strValue;
		int intValue;
		float floatValue;
		Vec2 vec2Data;
		IntVec2 intVec2Data;
		int type;

		std::string betweenString;
		std::string beforeString;
	};

	std::unordered_map<std::string, int> dataIndices;
	std::vector<Data> persistentData;

	int getDataIndex(const std::string& name)
	{
		auto index = dataIndices.find(name);
		if (index == dataIndices.end())
		{
			return -1;
		}
		return index->second;
	}

	Data& addData(const std::string& name)
	{
		dataIndices[name] = (int)persistentData.size();
		persistentData.resize(persistentData.size() + 1);
		int index = (int)persistentData.size() - 1;
		persistentData[index].name = name;
		return persistentData[index];
	}

	Data& getValidData(const std::string& name)
	{
		int index = getDataIndex(name);
		if (index == -1)
		{
			Data& data = addData(name);
			clearData(data);
			return data;
		}
		return persistentData[index];
	}

	Data& getData(const std::string& name)
	{
		int index = getDataIndex(name);
		if (index == -1)
		{
			return addData(name);
		}
		return persistentData[index];
	}

	Data& clearData(Data& data)
	{
		data.type = TypeInt;
		data.intValue = 0;
		data.floatValue = 0.0f;
		data.vec2Data.setZero();
		data.intVec2Data.setZero();
		return data;
	}

	Data& getEmptyData(const std::string& name)
	{
		Data& data = getData(name);
		clearData(data);
		return data;
	}

	void setValue(Data& data, int value)
	{
		data.intValue = value;
		data.type = TypeInt;
	}
	void setValue(Data& data, float value)
	{
		data.floatValue = value;
		data.type = TypeFloat;
	}
	void setValue(Data& data, const Vec2& value)
	{
		data.vec2Data = value;
		data.type = TypeVec2;
	}
	void setValue(Data& data, const IntVec2& value)
	{
		data.intVec2Data = value;
		data.type = TypeIntVec2;
	}
	void setValue(Data& data, const std::string& value)
	{
		data.strValue = value;
		data.type = TypeString;
	}

	template <class T>
	int setDataIfNotExistIndex(const std::string& name, const T& value)
	{
		int index = getDataIndex(name);
		if (index == -1)
		{
			Data& data = addData(name);
			clearData(data);
			setValue(data, value);
			return (int)persistentData.size() - 1;
		}
		return index;
	}

	void loadFromFile(const std::string& path)
	{
		std::string content;
		bool hasFile = textFileRead(path, content);
		if (hasFile)
		{
			loadFromString(content);
		}
	}

	void loadFromString(const std::string& content)
	{
		// this is mess :(
		// works though
		bool err = false;
		const char* c = content.c_str();
		while (*c != '\0')
		{
			const char* begin = c;
			while (*c)
			{
				if (*c == '\t' || *c == ' ' || *c == '\n' || *c == '\r')
				{
					c++;
				}
				else if (*c == '#')
				{
					c++;
					while (*c && *c != '\n')
					{
						c++;
					}
				}
				else
				{
					break;
				}
			}
			if (*c == '\0')
			{
				endString = begin;
				break;
			}
			const char* nameBegin = c;
			while (*c)
			{
				if (*c == '\n' || *c == '\r')
				{
					printf("ERROR\n");
					err = true;
					break;
				}
				else if (*c == ' ' || *c == '\t')
				{
					break;
				}
				c++;
			}

			if (*c == '\0' || err)
			{
				break;
			}

			const char* betweenBegin = c;

			while (*c)
			{
				if (*c == '\n' || *c == '\r')
				{
					printf("ERROR\n");
					err = true;
					break;
				}
				else if (*c == ' ' || *c == '\t')
				{
					c++;
				}
				else
				{
					break;
				}
			}

			
			if (*c == '\0' || err)
			{
				break;
			}

			const char* valueBegin = c;
			const char* valueBetweenBegin = c;
			int type;
			if (*c == '\"')
			{
				c++;
				type = TypeString;
				while (*c)
				{
					if (*c == '\n' || *c == '\r')
					{
						printf("ERROR\n");
						err = true;
						break;
					}
					else if (*c == '\"')
					{
						break;
					}
					c++;
				}

				if (*c == '\0' || err)
				{
					break;
				}
				c++;
			}
			else if (*c == '(')
			{
				type = TypeIntVec2;
				while (*c)
				{
					if (*c == '\n' || *c == '\r')
					{
						printf("ERROR\n");
						err = true;
						break;
					}
					else if (*c == ')')
					{
						break;
					}

					if (*c == ',')
					{
						valueBetweenBegin = c + 1;
					}
					if (*c == '.')
					{
						type = TypeVec2;
					}
					c++;
				}

				if (*c == '\0' || err)
				{
					break;
				}
				c++;
			}
			else
			{
				type = TypeInt;
				while (*c)
				{
					if (*c == '\n' || *c == '\r' || *c == ' ' || *c == '\t')
					{
						break;
					}
					if (*c == '.')
					{
						type = TypeFloat;
					}
					c++;
				}
			}
			std::string name(nameBegin, betweenBegin - nameBegin);
			if (dataIndices.find(name) != dataIndices.end())
			{
				printf("Warning: %s already defined\n", name.c_str());
			}
			Data& data = getEmptyData(name);
			if (type == TypeString)
			{
				std::string value(valueBegin + 1, c - valueBegin - 2);
				setValue(data, value);
			}
			else if (type == TypeInt)
			{
				int value = atoi(valueBegin);
				setValue(data, value);
			}
			else if (type == TypeFloat)
			{
				float value = (float)atof(valueBegin);
				setValue(data, value);
			}
			else if (type == TypeVec2)
			{
				Vec2 value;
				value[0] = (float)atof(valueBegin + 1);
				value[1] = (float)atof(valueBetweenBegin);
				setValue(data, value);
			}
			else if (type == TypeIntVec2)
			{
				IntVec2 value;
				value[0] = atoi(valueBegin + 1);
				value[1] = atoi(valueBetweenBegin);
				setValue(data, value);
			}

			data.beforeString = std::string(begin, nameBegin - begin);
			data.betweenString = std::string(betweenBegin, valueBegin - betweenBegin);

			/*
			char buffBefore[128];
			strncpy_s(buffBefore, begin, nameBegin - begin);
			char buffName[128];
			strncpy_s(buffName, nameBegin, betweenBegin - nameBegin);
			char buffBetween[128];
			strncpy_s(buffBetween, betweenBegin, valueBegin - betweenBegin);
			char buffValue[128];
			strncpy_s(buffValue, valueBegin, c - valueBegin);

			for (int i = 0; i < 128; i++)
			{
				if (buffBefore[i] == '\n') buffBefore[i] = 'n';
				if (buffBefore[i] == '\r') buffBefore[i] = 'r';

				if (buffName[i] == '\n') buffName[i] = 'n';
				if (buffName[i] == '\r') buffName[i] = 'r';

				if (buffBetween[i] == '\n') buffBetween[i] = 'n';
				if (buffBetween[i] == '\r') buffBetween[i] = 'r';

				if (buffValue[i] == '\n') buffValue[i] = 'n';
				if (buffValue[i] == '\r') buffValue[i] = 'r';
			}

			printf("b: [%s], n:[%s], t:[%s], v[%s] %d\n", buffBefore, buffName, buffBetween, buffValue, type);
			*/
		}

		FILE *f = fopen("out.txt", "w");
		for (auto& data : persistentData)
		{
			fprintf(f, "%s%s%s", data.beforeString.c_str(), data.name.c_str(), data.betweenString.c_str());
			if (data.type == TypeString)
			{
				fprintf(f, "\"%s\"", data.strValue.c_str());
			}
			else if (data.type == TypeInt)
			{
				fprintf(f, "%d", data.intValue);
			}
			else if (data.type == TypeFloat)
			{
				fprintf(f, "%.1f", data.floatValue);
			}
			else if (data.type == TypeVec2)
			{
				fprintf(f, "(%.1f, %.1f)", data.vec2Data[0], data.vec2Data[1]);
			}
			else if (data.type == TypeIntVec2)
			{
				fprintf(f, "(%d, %d)", data.intVec2Data[0], data.intVec2Data[1]);
			}
		}
		fprintf(f, "%s", endString.c_str());
		fclose(f);
	}

	std::string endString;
public:
	enum {
		TypeFloat,
		TypeInt,
		TypeString,
		TypeVec2,
		TypeIntVec2
	};

	cPersistent()
	{
		persistentData.reserve(256);
		loadFromFile("test.txt");
	}

	template <class T>
	Data& setDataIfNotExist(const std::string& name, const T& value)
	{
		int index = setDataIfNotExistIndex(name, value);
		return persistentData[index];
	}

	template <class T>
	void setData(int index, const T& value)
	{
		Data& data = persistentData[index];
		setValue(data, value);
	}

	template <class T>
	int setData(const std::string& name, const T& value)
	{
		int index = getDataIndex(name);
		Data& data = index == -1 ? getEmptyData(name) : persistentData[index];
		setValue(data, value);
		return index == -1 ? (int)persistentData.size() - 1 : index;
	}

	int getIntData(const std::string& name, int defaultValue = 0)
	{
		return setDataIfNotExist(name, defaultValue).intValue;
	}

	float getFloatData(const std::string& name, float defaultValue = 0.0f)
	{
		return setDataIfNotExist(name, defaultValue).floatValue;
	}

	const Vec2& getVec2Data(const std::string& name, const Vec2& defaultValue = Vec2::zero())
	{
		return setDataIfNotExist(name, defaultValue).vec2Data;
	}

	const IntVec2& getIntVec2Data(const std::string& name, const IntVec2& defaultValue = IntVec2::zero())
	{
		return setDataIfNotExist(name, defaultValue).intVec2Data;
	}

	const std::string& getStringData(const std::string& name, const std::string& defaultValue = "")
	{
		return setDataIfNotExist(name, defaultValue).strValue;
	}


	int getIntDataIndex(const std::string& name, int defaultValue = 0)
	{
		return setDataIfNotExistIndex(name, defaultValue);
	}

	int getStringDataIndex(const std::string& name, float defaultValue = 0.0f)
	{
		return setDataIfNotExistIndex(name, defaultValue);
	}

	int getStringDataIndex(const std::string& name, const Vec2& defaultValue = Vec2::zero())
	{
		return setDataIfNotExistIndex(name, defaultValue);
	}

	int getStringDataIndex(const std::string& name, const IntVec2& defaultValue = IntVec2::zero())
	{
		return setDataIfNotExistIndex(name, defaultValue);
	}

	int getStringDataIndex(const std::string& name, const std::string& defaultValue = "")
	{
		return setDataIfNotExistIndex(name, defaultValue);
	}


	int getIntData(int index)
	{
		return persistentData[index].intValue;
	}

	float getFloatData(int index)
	{
		return persistentData[index].floatValue;
	}

	const Vec2& getVec2Data(int index)
	{
		return persistentData[index].vec2Data;
	}

	const IntVec2& getIntVec2Data(int index)
	{
		return persistentData[index].intVec2Data;
	}

	const std::string& getStringData(int index)
	{
		return persistentData[index].strValue;
	}
};