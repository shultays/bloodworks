
#include "steam_api.h" 
#include "json.h" 

#include <stdlib.h> 
#include <windows.h> 
#include <string>

#include <iostream>
#include <fstream>

#define ID 717990

#define APPIDFILE "appid.txt"

bool textFileRead(std::string path, std::string &data)
{
	data = "";
	std::ifstream t(path);
	if (!t.good()) {
		return false;
	}
	t.seekg(0, std::ios::end);
	data.reserve((unsigned int)t.tellg());
	t.seekg(0, std::ios::beg);

	data.assign((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());
	return true;
}


class cSteamWorkshop
{
public:

	cSteamWorkshop() 
	{
		fin = false;
		steamInited = false;
		jsonRead = false;
	}

	void init()
	{
		steamInited = SteamAPI_Init();
	}

	bool fin;
	bool steamInited;

	std::string path;

	std::string title;
	std::string description;
	std::string picture;
	std::string meta;
	std::string change_note;
	std::vector<std::string> tags;
	bool jsonRead;
	bool ReadJson()
	{
		if (jsonRead)
		{
			return true;
		}
		jsonRead = true;

		std::string data;

		if (!textFileRead(path + "steam_mod_info.json", data))
		{
			printf("add steam_mod_info.json to set up a mod\n");
			return false;
		}

		nlohmann::json json;

		try
		{
			json = nlohmann::json::parse(data.c_str());
		}
		catch (...) 
		{
			printf("error parsing json file\n");
			return false;
		}

		if (json.count("title"))
		{
			title = json["title"].get<std::string>();
		}
		else
		{
			printf("Enter a title\n");
			return false;
		}

		if (json.count("description"))
		{
			description = json["description"].get<std::string>();
		}
		else
		{
			printf("Enter a description\n");
			return false;
		}

		if (json.count("picture"))
		{
			picture = json["picture"].get<std::string>();
		}
		else
		{
			printf("Enter a picture path\n");
			return false;
		}

		if (json.count("meta"))
		{
			meta = json["meta"].get<std::string>();
		}

		if (json.count("change_note"))
		{
			change_note = json["change_note"].get<std::string>();
		}
		else
		{
			change_note = "no notes";
		}

		if (json.count("tags"))
		{
			for (auto k : json["tags"])
			{
				tags.push_back(k.get<std::string>());
			}
		}

		return true;
	}

	void CreateMod(const std::string& path)
	{
		this->path = path;

		if (!ReadJson())
		{
			fin = true;
			return;
		}

		std::ifstream myfile;
		myfile.open(path + APPIDFILE);

		if (myfile.good())
		{
			AppId_t m_unAppID;

			if (myfile >> m_unAppID)
			{
				Update(m_unAppID);
			}
			else
			{
				printf("Couldn't read app id\n");
				return;
			}
		}
		else
		{
			printf("Creating new mod...\n");
			SteamAPICall_t Key = SteamUGC()->CreateItem(ID, k_EWorkshopFileTypeCommunity);
			OnCreateEvent.Set(Key, this, &cSteamWorkshop::OnCreate);
		}
	}

	CCallResult<cSteamWorkshop, CreateItemResult_t> OnCreateEvent;
	void OnCreate(CreateItemResult_t *result, bool failed)
	{
		createResult = *result;

		if (!failed)
		{
			std::ofstream myfile;
			myfile.open(path + APPIDFILE);
			myfile << createResult.m_nPublishedFileId;
			myfile.close();
		}
		if (result->m_bUserNeedsToAcceptWorkshopLegalAgreement)
		{
			std::stringstream ss;
			ss << "steam://url/CommunityFilePage/" << result->m_nPublishedFileId;
			SteamFriends()->ActivateGameOverlayToWebPage(ss.str().c_str());
			printf("\n\nYou need to agree with Steam Workshop Legal Agreement first to make your mod visible to public. Enter following URL to your browser for the agreement.\n\n%s\n\n", ss.str().c_str());
			system("pause");
		}
		if (!failed)
		{
			std::ofstream myfile;
			myfile.open(path + APPIDFILE);
			myfile << createResult.m_nPublishedFileId;
			myfile.close();

			Update(createResult.m_nPublishedFileId);
		}
	}
	CreateItemResult_t createResult;

	void Update(PublishedFileId_t m_unAppID)
	{
		if (!ReadJson())
		{
			fin = true;
			return;
		}

		UGCUpdateHandle_t Key = SteamUGC()->StartItemUpdate(ID, m_unAppID);

		SteamUGC()->SetItemTitle(Key, title.c_str());
		SteamUGC()->SetItemDescription(Key, description.c_str());
		if (meta.size())
		{
			SteamUGC()->SetItemMetadata(Key, meta.c_str());
		}

		SteamUGC()->SetItemVisibility(Key, k_ERemoteStoragePublishedFileVisibilityPublic);

		if (tags.size())
		{
			SteamParamStringArray_t tagData;
			const char *p[100];

			int t = 0;
			for (auto& k : tags)
			{
				p[t++] = k.c_str();
			}
			tagData.m_nNumStrings = t;
			tagData.m_ppStrings = p;
			SteamUGC()->SetItemTags(Key, &tagData);
		}

		SteamUGC()->SetItemPreview(Key, (path + picture).c_str());
		SteamUGC()->SetItemContent(Key, path.c_str());

		printf("Uploading...\n");

		SteamAPICall_t Key2 = SteamUGC()->SubmitItemUpdate(Key, change_note.c_str());
		OnUpdateEvent.Set(Key2, this, &cSteamWorkshop::OnUpdate);
	}

	void OnUpdate(SubmitItemUpdateResult_t *result, bool failed)
	{
		updateResult = *result;

		if (!failed && updateResult.m_eResult == k_EResultOK)
		{
			printf("Success!\n");
		}
		else
		{
			printf("Error\n");
		}
		fin = true;
	}

	CCallResult<cSteamWorkshop, SubmitItemUpdateResult_t> OnUpdateEvent;
	SubmitItemUpdateResult_t updateResult;
} steam;


int main(int argn, const char* argv[])
{
	std::string path = "";

	TCHAR full_path[MAX_PATH] = "";
	if (argn > 1)
	{
		path = argv[1];
	}
	else
	{
		printf("enter full path to mod folder: ");
		fgets(full_path, 100, stdin);
		path = full_path;
	}

	if (path.size())
	{
		if (path[path.size() - 1] == '\n')
		{
			path.resize(path.size() - 1);
		}

		if (path[path.size() - 1] != '\\')
		{
			path = path + "\\";
		}
	}
	
	printf("full path: %s\n", path.c_str());

	steam.init();

	if (!steam.steamInited)
	{
		printf("Error initting steam\n");
	}
	else
	{
		steam.CreateMod(path);
		while (!steam.fin)
		{
			Sleep(50);
			SteamAPI_RunCallbacks();
		}
	}

	system("pause");
}