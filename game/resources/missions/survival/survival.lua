
function Survival.init()
	local theSeed = os.time()
	math.randomseed( theSeed )
	missionData = {}
	missionData.ignoreMonsterCount = 0
    missionData.extraMin = 0.0
    missionData.lastSpawnTime = 0.0
	missionData.lastBossSpawn = -500
	missionData.perkPerLevel = 3
	missionData.firstKill = true
	missionData.firstTick = true
	missionData.spawnWeaponOnFirstKill = true
	missionData.timeToCalcSpawn = -0.1
	if DEBUG then
		missionData.maxMonster = 20
	else
		missionData.maxMonster = 650
		spawn = 50
	end
	missionData.curMaxMonster = missionData.maxMonster
	
	
end

function Survival.onTick()
    local min = missionTime / 60.0
	
	local monsterTypeCount = getAllMonsterTypeCount()
	
	if missionData.firstTick then
		for i = 0, monsterTypeCount - 1 do
			local monsterType = getMonsterTypeAt(i)
			if monsterType.scriptTable.onMissionLoad ~= nil then
				monsterType.scriptTable.onMissionLoad(missionData)
			end
		end
	end
	
	missionData.timeToCalcSpawn = missionData.timeToCalcSpawn - dt
	
	if missionData.timeToCalcSpawn < 0.0 then
		missionData.timeToCalcSpawn = math.random() * 0.5 + 0.5

		missionData.spawnRates = {}
		local monsterTypeCount = getAllMonsterTypeCount()
		missionData.totalChanceSpawn = 0.0
		
		local m = ""
		for i = 0, monsterTypeCount - 1 do
			local monsterType = getMonsterTypeAt(i)
			if monsterType.scriptTable.spawnChanceInMission ~= nil then
				local chance = monsterType.scriptTable.spawnChanceInMission(missionData, min)
				missionData.totalChanceSpawn = missionData.totalChanceSpawn + chance
				missionData.spawnRates[monsterType.name] = chance
			end
		end
	end
	
	if missionData.firstTick then
		local spawn
		if DEBUG then
			spawn = 10
		else
			spawn = 50
		end
		
		for i = 1, spawn do
			local pos = getRandomPosition( {canBeEdge=true, notOnScreen=true, notNearPlayer=true, notNearMonsters=true, playerRange=400.0})
			local monster = addRandomMonster()
			monster.position = pos
			monster.moveAngle =  math.random() * math.pi * 2.0
		end
	end


	if isKeyPressed(keys.Escape) or isKeyPressed(keys.joystick_0_button_back) then
		if gotoMainMenu() then
			return
		end
	end

	if player.isDead then
		if isKeyPressed(keys.Space) then
			loadMission("Survival")
		end
		
		return
	end

	missionData.curMaxMonster = math.floor(lerp(55, missionData.maxMonster, clamp(min * 0.05)))
    if missionTime - missionData.lastSpawnTime > (0.7 - clamp(min * 0.05) * 0.5) and canSpawnMonster() and player.isDead == false then
        missionData.lastSpawnTime = missionTime
		local pos = getRandomPosition({canBeEdge=true, notNearPlayer=true, notNearMonsters=true, notOnScreen=true})
        local monster = addRandomMonster()
		monster.position = pos
		monster.moveAngle =  math.random() * math.pi * 2.0
    end
	
	missionData.firstTick = false
end


function Survival.onPlayerDied()
	local gameObject = addGameObject("FadeOutImage")
	gameObject.data.startTime = time
	gameObject.data.fadeOutStartTime = -1
	gameObject.data.fadeInDuration = 1.0
	gameObject:setLevel(RenderableLevel.GUI + 5)
	gameObject.data.renderable = gameObject:addText("You Died", "resources/fontData.txt")
	gameObject.data.renderable:setAlignment(RenderableAlignment.center)
	gameObject.data.renderable:setTextAlignment(TextAlignment.center)
	gameObject.data.renderable:setTextSize(120.0)
	gameObject:setPosition(Vec2.new(0, 50))
	
	gameObject = addGameObject("FadeOutImage")
	gameObject.data.startTime = time
	gameObject.data.fadeOutStartTime = -1
	gameObject.data.fadeInDuration = 1.0
	gameObject:setLevel(RenderableLevel.GUI + 5)
	gameObject.data.renderable = gameObject:addText("Press Space to Reset", "resources/fontData.txt")
	gameObject.data.renderable:setAlignment(RenderableAlignment.center)
	gameObject.data.renderable:setTextAlignment(TextAlignment.center)
	gameObject.data.renderable:setTextSize(32.0)
	gameObject:setPosition(Vec2.new(0, -40))
	
	gameObject = addGameObject("FadeOutImage")
	gameObject.data.startTime = time
	gameObject.data.fadeOutStartTime = -1
	gameObject.data.fadeInDuration = 1.0
	gameObject:setLevel(RenderableLevel.GUI + 5)
	gameObject.data.renderable = gameObject:addText("Esc to Exit", "resources/fontData.txt")
	gameObject.data.renderable:setAlignment(RenderableAlignment.center)
	gameObject.data.renderable:setTextAlignment(TextAlignment.center)
	gameObject.data.renderable:setTextSize(32.0)
	gameObject:setPosition(Vec2.new(0, -80))
	
end

function Survival.onMonsterDied(monster)
	if missionData.firstKill then
		missionData.firstKill = false
	end

	if missionData.spawnWeaponOnFirstKill == true then
		spawnRandomGun(monster.position)
		missionData.spawnWeaponOnFirstKill = false
	end
	
	if monster.data.ignoreForCount == true then
		monster.data.ignoreForCount = false
		missionData.ignoreMonsterCount = missionData.ignoreMonsterCount - 1 
	end
end

function Survival.onDebugTick()
	if isKeyReleased(keys.Home) then
		HomingOrb.spawn(player.position)
	end
	if isKeyReleased(keys.PageUp) then
		missionData.extraMin = missionData.extraMin + 0.5
		print("Extra Min " .. missionData.extraMin)
	end
	if isKeyReleased(keys.PageDown) then
		missionData.extraMin = missionData.extraMin - 0.5
		if missionData.extraMin < 0.0 then
			missionData.extraMin = 0.0
		end
		print("Extra Min " .. missionData.extraMin)
	end
	
	if isKeyReleased(keys.Delete) then	
		local count = getMonsterCount()
		
		for i = 0, count - 1 do
			local monster = getMonsterAtIndex(i)
			monster.experienceMultiplier = 0.0
			monster:doDamage(10000, Vec2.new(1.0, 0.0))
		end
	end
	
	if isKeyReleased(keys.Insert) then
		local t = missionData.maxMonster * 0.5 - getMonsterCount()
		for i = 1, t - 10 do
			local pos = getRandomPosition( {canBeEdge=true, notNearPlayer=true, notNearMonsters=true, playerRange=400.0})
			local monster = addRandomMonster()
			monster.position = pos
			monster.moveAngle =  math.random() * math.pi * 2.0
		end
	end
end