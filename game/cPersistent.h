#pragma once

#include <string>
#include <unordered_map>

#include "cTools.h"

class cPersistent
{
	struct Data
	{
		std::string strValue;
		int intValue;
		float floatValue;
		Vec2 vec2Data;
		IntVec2 intVec2Data;
		int type;
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
		return persistentData[persistentData.size() - 1];
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