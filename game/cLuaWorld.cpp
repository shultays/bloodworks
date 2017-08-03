#include "cLuaWorld.h"
#include "json.h"
#include <initializer_list>

#include <iostream>

void cLuaWorld::fillTableWithJson(nlohmann::json& root, sol::table& table)
{
	for (auto it = root.begin(); it != root.end(); ++it)
	{
		std::string name = it.key();
		auto& j = it.value();
		if (j.is_number_integer() || j.is_number_unsigned())
		{
			table[name] = j.get<int>();
		}
		else if (j.is_number_float())
		{
			table[name] = j.get<float>();
		}
		else if (j.is_string())
		{
			table[name] = j.get<std::string>();
		}
		else if (j.is_boolean())
		{
			table[name] = j.get<bool>();
		}
		else if (j.is_array())
		{
			int size = (int)j.size();
			if (size == 2 && j.at(0).is_number() && j.at(1).is_number())
			{
				table[name] = Vec2(j.at(0).get<float>(), j.at(1).get<float>());
			}
			else if (size == 4 && j.at(0).is_number() && j.at(1).is_number() && j.at(2).is_number() && j.at(3).is_number())
			{
				table[name] = Vec4(j.at(0).get<float>(), j.at(1).get<float>(), j.at(2).get<float>(), j.at(3).get<float>());
			}
			else
			{
				table[name] = create_table();
				table[name]["size"] = size;
				for (int i = 0; i < size; i++)
				{
					auto& j2 = j.at(i);
					if (j2.is_number_integer() || j2.is_number_unsigned())
					{
						table[name][i] = j2.get<int>();
					}
					else if (j2.is_number_float())
					{
						table[name][i] = j2.get<float>();
					}
					else if (j2.is_string())
					{
						table[name][i] = j2.get<std::string>();
					}
					else if (j2.is_boolean())
					{
						table[name][i] = j2.get<bool>();
					}
					else if (j2.is_object())
					{
						sol::table table2 = table[name][i] = create_table();
						fillTableWithJson(j, table2);
					}
					else
					{
						assert(!"invalid_type");
					}
				}
			}
		}
		else if (j.is_object())
		{
			sol::table table2 = table[name] = create_table();
			fillTableWithJson(j, table2);
		}
		else
		{
			assert(!"invalid_type");
		}

	}
}

sol::table cLuaWorld::createTableForJson(nlohmann::json& root)
{
	sol::table t = create_table();
	fillTableWithJson(root, t);
	return t;
}
