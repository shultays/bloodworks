#include "cPersistent.h"

int cPersistent::getDataIndex(const std::string& name)
{
	auto index = dataIndices.find(name);
	if (index == dataIndices.end())
	{
		return -1;
	}
	return index->second;
}

cPersistent::Data& cPersistent::addData(const std::string& name)
{
	dataIndices[name] = (int)persistentData.size();
	persistentData.resize(persistentData.size() + 1);
	int index = (int)persistentData.size() - 1;
	persistentData[index].name = name;
	return persistentData[index];
}

cPersistent::Data& cPersistent::getValidData(const std::string& name)
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

cPersistent::Data& cPersistent::getData(const std::string& name)
{
	int index = getDataIndex(name);
	if (index == -1)
	{
		return addData(name);
	}
	return persistentData[index];
}

cPersistent::Data& cPersistent::clearData(Data& data)
{
	data.type = TypeInt;
	data.intValue = 0;
	data.floatValue = 0.0f;
	data.vec2Value.setZero();
	data.intVec2Value.setZero();
	return data;
}

cPersistent::Data& cPersistent::getEmptyData(const std::string& name)
{
	Data& data = getData(name);
	clearData(data);
	return data;
}

void cPersistent::loadFromString(const std::string& content)
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
			bool stringBegin = false;
			while (*c)
			{
				if (*c == '\n' || *c == '\r')
				{
					printf("ERROR\n");
					err = true;
					break;
				}
				else if (*c == ')' && stringBegin == false)
				{
					break;
				}

				if (*c == ',')
				{
					valueBetweenBegin = c + 1;
				}
				if (*c == '.' && type != TypeStringList)
				{
					type = TypeVec2;
				}
				else if (*c == '\"')
				{
					type = TypeStringList;
					stringBegin = !stringBegin;
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
		else if (type == TypeStringList)
		{
			data.type = TypeStringList;
			const char* c2 = valueBegin + 1;
			while (true)
			{
				bool searchBegin = true;
				while (c2 < c && *c2 != ')')
				{
					if (*c2 == ' ' || *c2 == '\t' || *c2 == ',')
					{
						c2++;
					}
					else if (*c2 == '\"')
					{
						break;
					}
					else
					{
						printf("ERROR");
						err = true;
						break;
					}
				}

				if (*c2 == ')' || err)
				{
					break;
				}

				int i = 1;

				while (c2 + i < c)
				{
					if (c2[i] == '\"')
					{
						break;
					}
					i++;
				}

				if (c2[i] == '\"')
				{
					std::string value(c2 + 1, i - 1);
					data.strList.push_back(value);
					c2 += i + 1;
				}
				else
				{
					printf("ERROR");
					break;
				}
			}
		}

		data.beforeString = std::string(begin, nameBegin - begin);
		data.betweenString = std::string(betweenBegin, valueBegin - betweenBegin);
	}
}

void cPersistent::saveToFile()
{
	if (filePath.size() == 0)
	{
		return;
	}
	char buff[128];
	FILE *f;
	bool err = fopen_s(&f, filePath.c_str(), "w") != 0;
	if (err == false)
	{
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
				int len = sprintf_s(buff, sizeof(buff), "%f", data.floatValue) - 1;
				while (len > 1 && buff[len] == '0' && buff[len - 1] == '0') buff[len--] = '\0';
				fprintf(f, "%s", buff);
			}
			else if (data.type == TypeVec2)
			{
				fprintf(f, "(");
				int len = sprintf_s(buff, sizeof(buff), "%f", data.vec2Value[0]) - 1;
				while (len > 1 && buff[len] == '0' && buff[len - 1] == '0') buff[len--] = '\0';
				fprintf(f, "%s, ", buff);
				len = sprintf_s(buff, sizeof(buff), "%f", data.vec2Value[1]) - 1;
				while (len > 1 && buff[len] == '0' && buff[len - 1] == '0') buff[len--] = '\0';
				fprintf(f, "%s)", buff);
			}
			else if (data.type == TypeIntVec2)
			{
				fprintf(f, "(%d, %d)", data.intVec2Value[0], data.intVec2Value[1]);
			}
			else if (data.type == TypeStringList)
			{
				fprintf(f, "(");
				for (int i = 0; i < data.strList.size(); i++)
				{
					fprintf(f, "\"%s\"", data.strList[i].c_str());
					if (i < data.strList.size() - 1)
					{
						fprintf(f, ", ");
					}
				}
				fprintf(f, ")");
			}
		}
		fprintf(f, "%s", endString.c_str());
		fclose(f);
	}
}
