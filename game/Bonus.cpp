#include "Bonus.h"

#include "json.h"
#include "cTools.h"
#include "cGlobals.h"

using json = nlohmann::json;

Bonus::Bonus(const std::string& bonusDataFile)
{
	std::string jsonFile;
	textFileRead(bonusDataFile.c_str(), jsonFile);
	json j = json::parse(jsonFile.c_str());

	name = j["name"].get<std::string>();
	scriptName = j["scriptName"].get<std::string>();
	std::string scriptFile = j["scriptFile"].get<std::string>();

	lua[scriptName] = lua.create_table();
	lua.script_file(scriptFile);
}
