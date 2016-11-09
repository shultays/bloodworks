#pragma once

#include <string>

#include "Sol.h"
#include "cMat.h"
#include "cVec.h"

class Bloodworks;
class cRenderable;

class MissionController
{
	Bloodworks *bloodworks;

	std::string scriptPath;
	sol::table scriptTable;

	std::string name;

	static int nextGameObjectId;


	enum RenderableDataType
	{
		texture,
		text
	};

	struct RenderableData
	{
		RenderableDataType type;
		cRenderable *renderable;
		Vec2 pos;
		Vec2 scale;
		float rotation;
	};

	struct GameObject
	{
		int id;
		bool hasOnTick;
		bool hasBulletCollision;
		bool hasPlayerCollision;
		bool hasMonsterCollision;

		std::string script;
		sol::table gameObject;

		std::vector<RenderableData> renderables;
	};

	std::unordered_map<int, GameObject> gameObjects;
public:
	MissionController(){}
	void loadMissionController(const std::string& missionControllerData);
	void clear();
	void init(Bloodworks *bloodworks);
	void tick(float dt);
	int addGameObject(const std::string& name, const std::string& script, const sol::table& initArgs);
	void removeGameObject(int id);
	int addTextureToGameObject(int gameObject, const sol::table& params, const std::string& texture, const std::string& shader);
	int addTextToGameObject(int gameObject, const sol::table& params, const std::string& text, const std::string& font);
	void updateRenderableParams(int gameObject, int renderableId, const sol::table& params);
private:
	int addCustomBullet(const sol::table& params);
};