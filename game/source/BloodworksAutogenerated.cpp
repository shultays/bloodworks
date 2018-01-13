#include "Bloodworks.h"
#include "autogenerated.h"

#include "Coral.h"
#include "cGlobals.h"

void Bloodworks::startAutoGenerating()
{
#ifdef HAS_AUTO_GENERATED
	return;
#endif
	autoGeneratedFile.open("source/autogenerated.h");
	autoLoadJsons.clear();
	if (autoGeneratedFile.good())
	{
		autoGenerating = true;

		autoGeneratedFile << "#include <unordered_map>" "\n";
		autoGeneratedFile << "#include <vector>" "\n";
		autoGeneratedFile << "#define HAS_AUTO_GENERATED" "\n";
		autoGeneratedFile << "std::unordered_map<std::string, const char*> fileMap = {" "\n";
	}
	else if (Coral::isDebuggerPresent())
	{
		DebugBreak();
	}
}


void Bloodworks::stopAutoGenerating()
{
	if (autoGenerating)
	{
		autoGeneratedFile << "};" "\n";

		autoGeneratedFile << "std::vector<std::string> autoLoads = {" "\n";

		for (auto& t : autoLoadJsons)
		{
			autoGeneratedFile << "\"" << t << "\"," "\n";
		}

		autoGeneratedFile << "};" "\n";

		autoGeneratedFile.close();
		autoGenerating = false;
	}
}

void fixText(std::string& data)
{
	data = replaceAll(data, "\"", "\\\"");

	data = replaceAll(data, "\r", "");
	data = replaceAll(data, "\\n", "\\\\n\"\"");
	data = replaceAll(data, "\n", "\\n\"\"");
}

void Bloodworks::loadJsonFile(nlohmann::json& j, const std::string& filePath, bool loadInit)
{
	std::string data;
#ifdef HAS_AUTO_GENERATED
	if (loadAutoGenerated)
	{
		data = fileMap.at(filePath);
	}
	else
#endif
	{
		textFileRead(filePath, data);
	}

	j = nlohmann::json::parse(data.c_str());

	if (autoGenerating && loadInit )
	{
		autoLoadJsons.push_back(filePath);
	}
}

void Bloodworks::loadLuaFile(const std::string& scriptFilePath)
{
#ifdef HAS_AUTO_GENERATED
	if (loadAutoGenerated)
	{
		std::string data = fileMap.at(scriptFilePath);
		lua.script(data);
	}
	else
#endif
	{
		lua.script_file(scriptFilePath);
	}
}


void Bloodworks::loadResources()
{
#ifdef HAS_AUTO_GENERATED
	if (loadAutoGenerated)
	{
		for (auto& t : autoLoads)
		{
			nlohmann::json j;
			appendJson(this, j, t, true);
			DirentHelper::File f;
			size_t z = t.find_last_of( "/");
			f.folder = t.substr( 0,  z);
			fixFolderPath(f.folder);
			f.file = t.substr(z+1, t.size());
			parseJson(j, f);
		}
	}
	else
#endif
	{
		loadMod("resources");

		if (autoGenerating)
		{
			DirentHelper::Folder folder("resources");
			cVector<DirentHelper::File> files = folder.getAllFiles(true);

			for (auto& f : files)
			{
				if (f.isTypeOf("lua") || f.isTypeOf("json"))
				{

					std::string data;
					std::string scriptPath = f.folder + f.file;
					fixFilePath(scriptPath);
					textFileRead(scriptPath, data);

					fixText(data);
					autoGeneratedFile << "{\"" << scriptPath << "\", \"" << data << "\"}, " "\n";
				}
			}
		}
	}
}