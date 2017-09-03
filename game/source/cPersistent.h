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
		cVector<std::string> strList;

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
	cVector<Data> persistentData;

	std::string endString;
	std::string filePath;

	bool addExtraNewLine;
	bool isDirty;

	int getDataIndex(const std::string& name);
	Data& addData(const std::string& name);
	Data& getValidData(const std::string& name);
	Data& getData(const std::string& name);
	Data& clearData(Data& data);
	Data& getEmptyData(const std::string& name);

	void setValue(Data& data, bool value)
	{
		data.intValue = value ? 1 : 0;
		data.type = TypeInt;
		isDirty = true;
	}

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
	void setValue(Data& data, const cVector<std::string>& value)
	{
		data.strList = value;
		data.type = TypeStringList;
		isDirty = true;
	}

	template <class T>
	int setDataIfNotExistIndex(const std::string& name, const T& value, const char* comment = nullptr)
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
			if (comment)
			{
				data.beforeString += "\n# " + std::string(comment) + "\n";
				addExtraNewLine = true;
			}
			else if (addExtraNewLine)
			{
				data.beforeString += "\n";
				addExtraNewLine = false;
			}
			clearData(data);
			setValue(data, value);
			return persistentData.size() - 1;
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
		addExtraNewLine = false;
		persistentData.reserve(256);
	}

	~cPersistent()
	{
		check();
	}

	template <class T>
	Data& setDataIfNotExist(const std::string& name, const T& value, const char* comment = nullptr)
	{
		int index = setDataIfNotExistIndex(name, value, comment);
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
		return index == -1 ? persistentData.size() - 1 : index;
	}

	int getInt(const std::string& name, int defaultValue = 0, const char* comment = nullptr)
	{
		int index = setDataIfNotExistIndex(name, defaultValue, comment);
		return getInt(index);
	}

	bool getBool(const std::string& name, bool defaultValue = false, const char* comment = nullptr)
	{
		int index = setDataIfNotExistIndex(name, defaultValue, comment);
		return getBool(index);
	}

	float getFloat(const std::string& name, float defaultValue = 0.0f, const char* comment = nullptr)
	{
		int index = setDataIfNotExistIndex(name, defaultValue, comment);
		return getFloat(index);
	}

	Vec2 getVec2(const std::string& name, const Vec2& defaultValue = Vec2::zero(), const char* comment = nullptr)
	{
		int index = setDataIfNotExistIndex(name, defaultValue, comment);
		return getVec2(index);
	}

	IntVec2 getIntVec2(const std::string& name, const IntVec2& defaultValue = IntVec2::zero(), const char* comment = nullptr)
	{
		int index = setDataIfNotExistIndex(name, defaultValue, comment);
		return getIntVec2(index);
	}

	const std::string& getString(const std::string& name, const std::string& defaultValue = "", const char* comment = nullptr)
	{
		return setDataIfNotExist(name, defaultValue, comment).strValue;
	}

	int getIndex(const std::string& name, const char* comment = nullptr)
	{
		return setDataIfNotExistIndex(name, 0, comment);
	}

	int getIndex(const std::string& name, int defaultValue, const char* comment = nullptr)
	{
		return setDataIfNotExistIndex(name, defaultValue, comment);
	}

	int getIndex(const std::string& name, float defaultValue, const char* comment = nullptr)
	{
		return setDataIfNotExistIndex(name, defaultValue, comment);
	}

	int getIndex(const std::string& name, const Vec2& defaultValue, const char* comment = nullptr)
	{
		return setDataIfNotExistIndex(name, defaultValue, comment);
	}

	int getIndex(const std::string& name, const IntVec2& defaultValue, const char* comment = nullptr)
	{
		return setDataIfNotExistIndex(name, defaultValue, comment);
	}

	int getIndex(const std::string& name, const std::string& defaultValue, const char* comment = nullptr)
	{
		return setDataIfNotExistIndex(name, defaultValue, comment);
	}

	bool getBool(int index)
	{
		Data& data = persistentData[index];
		if (data.type == TypeFloat)
		{
			return data.floatValue != 0.0f;
		}
		return data.intValue != 0;
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

	cVector<std::string>& getStringList(int index)
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
		return persistentData.size();
	}

	const std::string& getName(int index)
	{
		return persistentData[index].name;
	}
};