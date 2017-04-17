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
		Vec2 vec2Value;
		IntVec2 intVec2Value;
		int type;
		std::vector<std::string> strList;

		std::string betweenString;
		std::string beforeString;
	};

	enum 
	{
		TypeFloat,
		TypeInt,
		TypeString,
		TypeVec2,
		TypeIntVec2,
		TypeStringList
	};
	std::unordered_map<std::string, int> dataIndices;
	std::vector<Data> persistentData;

	std::string endString;
	std::string filePath;

	bool isDirty;

	int getDataIndex(const std::string& name);
	Data& addData(const std::string& name);
	Data& getValidData(const std::string& name);
	Data& getData(const std::string& name);
	Data& clearData(Data& data);
	Data& getEmptyData(const std::string& name);

	void setValue(Data& data, int value)
	{
		data.intValue = value;
		data.type = TypeInt;
		isDirty = true;
	}
	void setValue(Data& data, float value)
	{
		data.floatValue = value;
		data.type = TypeFloat;
		isDirty = true;
	}
	void setValue(Data& data, const Vec2& value)
	{
		data.vec2Value = value;
		data.type = TypeVec2;
		isDirty = true;
	}
	void setValue(Data& data, const IntVec2& value)
	{
		data.intVec2Value = value;
		data.type = TypeIntVec2;
		isDirty = true;
	}
	void setValue(Data& data, const std::string& value)
	{
		data.strValue = value;
		data.type = TypeString;
		isDirty = true;
	}

	template <class T>
	int setDataIfNotExistIndex(const std::string& name, const T& value)
	{
		int index = getDataIndex(name);
		if (index == -1)
		{
			Data& data = addData(name);
			int space = 16 - (int)name.length();
			space = max(1, space);
			for (int i = 0; i < space; i++)
			{
				data.betweenString += " ";
			}
			data.beforeString = "\n";
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
			isDirty = false;
		}
	}

	void loadFromString(const std::string& content);
	void saveToFile();
public:

	cPersistent()
	{
		isDirty = false;
		persistentData.reserve(256);
	}

	~cPersistent()
	{
		check();
	}

	template <class T>
	Data& setDataIfNotExist(const std::string& name, const T& value)
	{
		int index = setDataIfNotExistIndex(name, value);
		return persistentData[index];
	}

	template <class T>
	void set(int index, const T& value)
	{
		Data& data = persistentData[index];
		setValue(data, value);
	}

	template <class T>
	int set(const std::string& name, const T& value)
	{
		int index = getDataIndex(name);
		Data& data = index == -1 ? getEmptyData(name) : persistentData[index];
		setValue(data, value);
		return index == -1 ? (int)persistentData.size() - 1 : index;
	}

	int getInt(const std::string& name, int defaultValue = 0)
	{
		int index = setDataIfNotExistIndex(name, defaultValue);
		return getInt(index);
	}

	float getFloat(const std::string& name, float defaultValue = 0.0f)
	{
		int index = setDataIfNotExistIndex(name, defaultValue);
		return getFloat(index);
	}

	Vec2 getVec2(const std::string& name, const Vec2& defaultValue = Vec2::zero())
	{
		int index = setDataIfNotExistIndex(name, defaultValue);
		return getVec2(index);
	}

	IntVec2 getIntVec2(const std::string& name, const IntVec2& defaultValue = IntVec2::zero())
	{
		int index = setDataIfNotExistIndex(name, defaultValue);
		return getIntVec2(index);
	}

	const std::string& getString(const std::string& name, const std::string& defaultValue = "")
	{
		return setDataIfNotExist(name, defaultValue).strValue;
	}

	int getIndex(const std::string& name)
	{
		return setDataIfNotExistIndex(name, 0);
	}

	int getIndex(const std::string& name, int defaultValue)
	{
		return setDataIfNotExistIndex(name, defaultValue);
	}

	int getIndex(const std::string& name, float defaultValue)
	{
		return setDataIfNotExistIndex(name, defaultValue);
	}

	int getIndex(const std::string& name, const Vec2& defaultValue)
	{
		return setDataIfNotExistIndex(name, defaultValue);
	}

	int getIndex(const std::string& name, const IntVec2& defaultValue)
	{
		return setDataIfNotExistIndex(name, defaultValue);
	}

	int getIndex(const std::string& name, const std::string& defaultValue)
	{
		return setDataIfNotExistIndex(name, defaultValue);
	}

	int getInt(int index)
	{
		Data& data = persistentData[index];
		if (data.type == TypeFloat)
		{
			return (int)data.floatValue;
		}
		return data.intValue;
	}

	float getFloat(int index)
	{
		Data& data = persistentData[index];
		if (data.type == TypeInt)
		{
			return (float)data.intValue;
		}
		return data.floatValue;
	}

	Vec2 getVec2(int index)
	{
		Data& data = persistentData[index];
		if (data.type == TypeIntVec2)
		{
			return data.intVec2Value.toVec();
		}
		return data.vec2Value;
	}

	IntVec2 getIntVec2(int index)
	{
		Data& data = persistentData[index];
		if (data.type == TypeVec2)
		{
			return IntVec2((int)data.vec2Value[0], (int)data.vec2Value[1]);
		}
		return data.intVec2Value;
	}

	const std::string& getString(int index)
	{
		return persistentData[index].strValue;
	}

	std::vector<std::string>& getStringList(int index)
	{
		return persistentData[index].strList;
	}

	void setFileBackup(const std::string& file)
	{
		loadFromFile(file);
		this->filePath = file;
	}

	void check()
	{
		if (isDirty)
		{
			isDirty = false;
			saveToFile();
		}
	}

	int size() const
	{
		return (int)persistentData.size();
	}

	const std::string& getName(int index)
	{
		return persistentData[index].name;
	}
};