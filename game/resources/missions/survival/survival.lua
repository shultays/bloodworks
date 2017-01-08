
function addRandomMonster()
    local m = "Alien"
    local min = missionTime / 60.0 +  math.random() * 1.5 + Survival.extraMin
    if math.random() < 0.1 + clamp(min * 0.5) * 0.2 then
        m = "Spider"
    end
    local monster = addMonster(m)
    monster.data.randomMove = (math.random() > (0.05 + clamp(min * 0.2) * 0.5))
    monster.data.playerSeeRange = monster.data.playerSeeRange * (1.0 +  clamp(min * 0.1) * 2.0)
    monster.data.maxMoveSpeed =  monster.data.maxMoveSpeed * (1.0 + clamp(min * 0.05) * 0.75)
    monster.data.maxRotateSpeed =  monster.data.maxRotateSpeed * (1.0 + clamp(min * 0.05) * 1.0)
	
	
	monster.data.hitWaitTime = monster.data.hitWaitTime  * (1.0 - clamp(min * 0.1) * 0.8)
	monster.data.hitInterval = monster.data.hitInterval  * (1.0 - clamp(min * 0.1) * 0.4)
	monster.data.minDamage = math.floor(monster.data.minDamage * (1.0 + min * 0.2))
	monster.data.maxDamage = math.floor(monster.data.maxDamage * (1.0 + min * 0.2))
	
	monster.hitPoint = math.floor(monster.hitPoint * (1.0 + clamp(min * 0.05) * 1.0))
	
	if Survival.lastBossSpawn + 35.0 - clamp(min/7) * 15 < missionTime then
		Survival.lastBossSpawn = missionTime
		makeBoss(monster)
	end
    return monster
end

function makeBoss(monster)
	monster.hitPoint = math.floor(monster.hitPoint * 1.5)
	
	monster.data.minDamage = math.floor(monster.data.minDamage * 1.1)
	monster.data.maxDamage = math.floor(monster.data.maxDamage * 1.1)
	
	monster.data.maxMoveSpeed = monster.data.maxMoveSpeed * 1.05
	monster.data.randomMove = false
	monster.data.playerSeeRange = monster.data.playerSeeRange * 1.5
	
	local t = math.random() * 4

	if t < 1 then
		monster.hitPoint = monster.hitPoint * 7
		monster.colorMultiplier:addBuff(Vec4.new(0.9, 0.8, 0.3, 1.0))
		monster:setScale(monster.scale * 1.25)
	elseif t < 2 then
		monster.colorMultiplier:addBuff(Vec4.new(0.5, 0.5, 0.5, 0.5))
		monster:setScale(monster.scale * 0.85)
	elseif t < 3 then
		monster.colorMultiplier:addBuff(Vec4.new(1.0, 0.3, 0.3, 1.0))
		monster.data.minDamage = math.floor(monster.data.minDamage * 2.0)
		monster.data.maxDamage = math.floor(monster.data.maxDamage * 2.0)
	else 
		monster.colorMultiplier:addBuff(Vec4.new(0.2, 0.7, 1.0, 1.0))
		monster.data.maxMoveSpeed = monster.data.maxMoveSpeed * 1.55
		monster.data.minDamage = math.floor(monster.data.minDamage * 0.8)
		monster.data.maxDamage = math.floor(monster.data.maxDamage * 0.8)
	end
end


function Survival.init()

    Survival.extraMin = 0.0
    Survival.lastSpawnTime = 0.0
	Survival.lastBossSpawn = -500
	local spawn
	if DEBUG then
		Survival.maxMonster = 10
		spawn = 10
	else
		Survival.maxMonster = 550
		spawn = 50
	end
	
	for i = 1, spawn do
		local pos = getRandomPosition( {canBeEdge=true, notOnScreen=true, notNearPlayer=true, notNearMonsters=true, playerRange=400.0})
        local monster = addRandomMonster()
		monster.position = pos
		monster.moveAngle =  math.random() * math.pi * 2.0
	end
    
	Survival.perkPerLevel = 3
	missionData = Survival
end

function Survival.onTick()
	if isKeyPressed(keys.escape) or isKeyPressed(keys.joystick_0_button_back) then
		if gotoMainMenu() then
			return
		end
	end

	if player.isDead then
		if isKeyPressed(keys.space) then
			loadMission("Survival")
		end
		
		return
	end

	if isKeyReleased(keys.home) then
		dumpTable(_G)
	end
	
	if isKeyReleased(keys.pageup) then
		Survival.extraMin = Survival.extraMin + 0.5
		print("Extra Min " .. Survival.extraMin)
	end
	if isKeyReleased(keys.pagedown) then
		Survival.extraMin = Survival.extraMin - 0.5
		if Survival.extraMin < 0.0 then
			Survival.extraMin = 0.0
		end
		print("Extra Min " .. Survival.extraMin)
	end
	
	if isKeyReleased(keys.delete) then	
		local count = getMonsterCount()
		
		for i = 0, count - 1 do
			local monster = getMonsterAtIndex(i)
			monster.experience = 0
			monster:doDamage(10000, Vec2.new(1.0, 0.0))
		end
	end
	
	if isKeyReleased(keys.insert) then
		local t = Survival.maxMonster - getMonsterCount()
		for i = 1, t - 10 do
			local pos = getRandomPosition( {canBeEdge=true, notNearPlayer=true, notNearMonsters=true, playerRange=400.0})
			local monster = addRandomMonster()
			monster.position = pos
			monster.moveAngle =  math.random() * math.pi * 2.0
		end
	end
	
    local min = missionTime / 60.0
	local curMaxMonster = math.floor(lerp(55, Survival.maxMonster, clamp(min * 0.15)))
	
    if missionTime - Survival.lastSpawnTime > (0.7 - clamp(min * 0.15) * 0.5) and getMonsterCount() < curMaxMonster and player.isDead == false then
        Survival.lastSpawnTime = missionTime
		local pos = getRandomPosition({canBeEdge=true, notNearPlayer=true, notNearMonsters=true, notOnScreen=true})
        local monster = addRandomMonster()
		monster.position = pos
		monster.moveAngle =  math.random() * math.pi * 2.0
    end
end


function Survival.onPlayerDied()
	local gameObject = addGameObject("FadeOutImage")
	gameObject.data.startTime = time
	gameObject.data.fadeOutStartTime = -1
	gameObject.data.fadeInDuration = 1.0
	gameObject:setLevel(RenderableLevel.gui + 5)
	gameObject.data.renderable = gameObject:addText("You Died", "resources/fontData.txt")
	gameObject.data.renderable.alignment = RenderableAlignment.center
	gameObject.data.renderable.textSize = 120
	gameObject.data.renderable.color = 0
	gameObject.data.renderable:update()
	gameObject:setPosition(Vec2.new(0, 50))
	
	gameObject = addGameObject("FadeOutImage")
	gameObject.data.startTime = time
	gameObject.data.fadeOutStartTime = -1
	gameObject.data.fadeInDuration = 1.0
	gameObject:setLevel(RenderableLevel.gui + 5)
	gameObject.data.renderable = gameObject:addText("Press Space to Reset", "resources/fontData.txt")
	gameObject.data.renderable.alignment = RenderableAlignment.center
	gameObject.data.renderable.textSize = 32
	gameObject.data.renderable.color = 0
	gameObject.data.renderable:update()
	gameObject:setPosition(Vec2.new(0, -40))
	
	gameObject = addGameObject("FadeOutImage")
	gameObject.data.startTime = time
	gameObject.data.fadeOutStartTime = -1
	gameObject.data.fadeInDuration = 1.0
	gameObject:setLevel(RenderableLevel.gui + 5)
	gameObject.data.renderable = gameObject:addText("Esc to Exit", "resources/fontData.txt")
	gameObject.data.renderable.alignment = RenderableAlignment.center
	gameObject.data.renderable.textSize = 32
	gameObject.data.renderable.color = 0
	gameObject.data.renderable:update()
	gameObject:setPosition(Vec2.new(0, -80))
	
end
