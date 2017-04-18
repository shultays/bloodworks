
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
	
	monster.experienceMultiplier = math.random() * 0.4 + 0.8
	
	monster.hitPoint = math.floor(monster.hitPoint * (1.0 + clamp(min * 0.05) * 1.0))
	
	monster.experienceMultiplier = 0.9 + math.random() * 0.2
	monster.scoreMultiplier = 0.9 + math.random() * 0.2
	
	if Survival.lastBossSpawn + 35.0 - clamp(min/7) * 15 < missionTime then
		Survival.lastBossSpawn = missionTime
		makeBoss(monster)
	elseif monster.data.shootsBullets == false and math.random() > 0.98 - clamp(min * 0.2) * 0.05 then
		monster.data.shootsBullets = true
		monster.data.bulletMinDamage = math.floor(monster.data.bulletMinDamage * (1.0 + min * 0.3))
		monster.data.bulletMaxDamage = math.floor(monster.data.bulletMaxDamage * (1.0 + min * 0.4))
		monster.data.bulletRate = monster.data.bulletRate - clamp(min * 0.1) * 2.0
		monster.data.bulletRandom = monster.data.bulletRandom - clamp(min * 0.15) * 0.2
		monster.colorMultiplier:addBuff(Vec4.new(0.8, 0.95, 0.8, 1.0))
	else
		local r = 0.9 + 0.1 * math.random()
		local g = 0.9 + 0.1 * math.random()
		local b = 0.9 + 0.1 * math.random()
		local ra = math.random()
		if ra < 0.333 then
			r = 1.0
		elseif ra < 0.666 then
			g = 1.0
		else
			b = 1.0
		end
		
		monster.colorMultiplier:addBuff(Vec4.new(r, g, b, 1.0))
	end
	
    return monster
end

function makeBoss(monster)
    local min = missionTime / 60.0 +  math.random() * 1.5 + Survival.extraMin
	monster.hitPoint = math.floor(monster.hitPoint * 1.5)
	
	monster.data.minDamage = math.floor(monster.data.minDamage * 1.1)
	monster.data.maxDamage = math.floor(monster.data.maxDamage * 1.1)
	
	monster.data.maxMoveSpeed = monster.data.maxMoveSpeed * 1.05
	monster.data.randomMove = false
	monster.data.playerSeeRange = monster.data.playerSeeRange * 1.5
	
	monster.experienceMultiplier = 5.0 + math.random() * 2.0
	monster.scoreMultiplier = 5.0 + math.random() * 2.0
	
	local t = math.random(11)
	if t == 1 then
		monster.hitPoint = monster.hitPoint * 7
		monster.colorMultiplier:addBuff(Vec4.new(0.9, 0.8, 0.3, 1.0))
		monster:setScale(1.0 + math.random() * 0.3)
	elseif t == 2 then
		monster.colorMultiplier:addBuff(Vec4.new(0.5, 0.5, 0.5, 0.5))
		monster:setScale(monster.scale * 0.85)
	elseif t == 3 then
		monster.colorMultiplier:addBuff(Vec4.new(1.0, 0.3, 0.3, 1.0))
		monster.data.minDamage = math.floor(monster.data.minDamage * 2.0)
		monster.data.maxDamage = math.floor(monster.data.maxDamage * 2.0)
	elseif t == 4 then
		monster.colorMultiplier:addBuff(Vec4.new(0.2, 0.7, 1.0, 1.0))
		monster.data.maxMoveSpeed = monster.data.maxMoveSpeed * 1.55
		monster.data.minDamage = math.floor(monster.data.minDamage * 0.8)
		monster.data.maxDamage = math.floor(monster.data.maxDamage * 0.8)
	elseif t == 5 then
		monster.colorMultiplier:addBuff(Vec4.new(0.2, 0.7, 0.3, 1.0))
		monster.data.shootsBullets = true
		monster.data.bulletMinDamage = math.floor(monster.data.bulletMinDamage * (2.0 + min * 1.5))
		monster.data.bulletMaxDamage = math.floor(monster.data.bulletMaxDamage * (2.0 + min * 1.5))
		monster.data.bulletRate = 1.5 - clamp(min * 0.1) * 0.8
		monster.data.bulletRandom = 0.2 - clamp(min * 0.15) * 0.15
	elseif t == 6 then
		monster.data.remainingLife = 3
		monster.colorMultiplier:addBuff(Vec4.new(0.7, 0.2, 0.7, 1.0))
		monster.scriptTable = shallowcopy(monster.scriptTable)
		monster:setScale(0.8 + math.random() * 0.3)
		monster.experienceMultiplier = monster.experienceMultiplier * 0.3
		monster.scoreMultiplier = monster.scoreMultiplier * 0.3
		monster.scriptTable.onKilled = function (monster)
			if monster.data.remainingLife > 0 then
				monster.data.remainingLife = monster.data.remainingLife - 1
				for i = 1,2 do
					local newMonster = addMonster(monster.monsterTemplate.name)
					newMonster.data.remainingLife = monster.data.remainingLife
					newMonster.position = monster.position
					newMonster:setScale(math.max(0.3, monster.scale * 0.80))
					newMonster.colorMultiplier:addBuff(Vec4.new(0.7, 0.2, 0.7, 1.0))
					newMonster.scriptTable = monster.scriptTable
					newMonster:copyIgnoreId(monster)
					
					newMonster.data.playerSeeRange = monster.data.playerSeeRange
					newMonster.data.maxMoveSpeed = monster.data.maxMoveSpeed * 0.8
					newMonster.data.maxRotateSpeed = monster.data.maxRotateSpeed

					newMonster.data.hitWaitTime = monster.data.hitWaitTime
					newMonster.data.hitInterval = monster.data.hitInterval
					newMonster.data.minDamage = monster.data.minDamage
					newMonster.data.maxDamage = monster.data.maxDamage

					newMonster.experienceMultiplier = monster.experienceMultiplier * 0.5
					newMonster.scoreMultiplier = monster.scoreMultiplier * 0.5

					newMonster.hitPoint = math.floor(monster.hitPoint * 0.5)
					
					newMonster.moveAngle = monster.moveAngle + math.pi * (i - 0.5)
				end
			end
		end
	elseif t == 7 then
		monster.colorMultiplier:addBuff(Vec4.new(0.2, 0.2, 0.2, 1.0))
		monster.scriptTable = shallowcopy(monster.scriptTable)
		monster:setScale(0.8 + math.random() * 0.3)
		monster.experienceMultiplier = monster.experienceMultiplier * 0.5
		monster.scoreMultiplier = monster.scoreMultiplier * 0.5
		monster.scriptTable.onKilled = function (monster)
			for i = 1,8 do
				local newMonster = addMonster(monster.monsterTemplate.name)
				newMonster.position = monster.position
				newMonster:setScale(math.max(0.3, monster.scale * 0.50))
				newMonster.colorMultiplier:addBuff(Vec4.new(0.2, 0.2, 0.2, 1.0))
				newMonster:copyIgnoreId(monster)
				
				newMonster.data.playerSeeRange = monster.data.playerSeeRange
				newMonster.data.maxMoveSpeed = monster.data.maxMoveSpeed * 1.3
				newMonster.data.maxRotateSpeed = monster.data.maxRotateSpeed

				newMonster.data.hitWaitTime = monster.data.hitWaitTime
				newMonster.data.hitInterval = monster.data.hitInterval
				newMonster.data.minDamage = monster.data.minDamage
				newMonster.data.maxDamage = monster.data.maxDamage

				newMonster.experienceMultiplier = monster.experienceMultiplier * 0.1
				newMonster.scoreMultiplier = monster.scoreMultiplier * 0.1

				newMonster.hitPoint = math.floor(monster.hitPoint * 0.3)
				newMonster.data.randomMove = true
				newMonster.moveAngle = monster.moveAngle + math.pi * i / 8
			end
		end
	elseif t == 8 then
		monster.colorMultiplier:addBuff(Vec4.new(2.0, 2.0, 2.0, 1.0))
		monster.data.oldTick = monster.scriptTable.onTick
		monster.scriptTable = shallowcopy(monster.scriptTable)
		monster.data.maxMoveSpeed = monster.data.maxMoveSpeed * 3.5
		monster.data.originalSpeed = monster.data.maxMoveSpeed
		monster.data.maxRotateSpeed =  monster.data.maxRotateSpeed * 3.0
		monster.data.hitWaitTime = monster.data.hitWaitTime * 0.1
		monster.scriptTable.onTick = function (monster)
			local diffToPlayer = player.position - monster.position 
			local distanceToPlayer = diffToPlayer:length()
			local angleToPlayer = diffToPlayer:getAngle()
			local a = fixAngle(angleToPlayer - player.aimAngle)
			if math.abs(a) > math.pi - 0.2 then
				monster.data.maxMoveSpeed = 0.0
				monster.animationSpeed = 0.0
				monster.moveSpeed = 0.0
			else
				monster.data.maxMoveSpeed = monster.data.originalSpeed
				monster.animationSpeed = 1.0
				monster.data.oldTick(monster)
			end
		end
	elseif t == 9 then
		monster.colorMultiplier:addBuff(Vec4.new(0.7, 1.7, 0.7, 1.0))
		monster.data.oldTick = monster.scriptTable.onTick
		monster.scriptTable = shallowcopy(monster.scriptTable)
		monster.scriptTable.onHit = function(monster, damage, args)
			local buffId = monster.colorMultiplier:addBuff(Vec4.new(1.0, 1.0, 1.0, 0.2))
			monster.colorMultiplier:setBuffDuration(buffId, 1.0)
			monster.data.lastHitTime = time
		end
		
		monster.scriptTable.shouldHit = function(monster, bullet)
			return monster.data.lastHitTime == nil or time - monster.data.lastHitTime > 1.0
		end
	elseif t == 10 then
		monster.data.blinkParticle = monster:addParticleSpawner("CriticalParticle", {});
		monster.colorMultiplier:addBuff(Vec4.new(0.7, 1.7, 1.7, 1.0))
		monster.hitPoint = math.floor(monster.hitPoint * 1.5)
		monster.scriptTable = shallowcopy(monster.scriptTable)
		monster.scriptTable.onHit = function(monster, damage, args)
			if monster.hitPoint > 0 then
				local t = math.random() * math.pi * 2.0
				local r = math.random() * 100.0 + 100.0
				local v = Vec2:new()
				v:setAngle(t)
				v = v * r
				for i=1,8 do
					monster:spawnParticle(monster.data.blinkParticle, {initialScale = 15.0, moveSpeed = 150.0})
				end
				monster.position = monster.position + v
				for i=1,8 do
					monster:spawnParticle(monster.data.blinkParticle, {initialScale = 15.0, moveSpeed = 150.0})
				end
				
				playSound({path = "resources/sounds/shimmer_1.ogg"})
			end
		end
	elseif t == 11 then
		monster.colorMultiplier:addBuff(Vec4.new(0.5, 0.3, 0.2, 1.0))
		monster.data.oldTick = monster.scriptTable.onTick
		monster.scriptTable = shallowcopy(monster.scriptTable)
		monster.moveSpeed = monster.moveSpeed * 0.5
		monster:setScale(0.8 + math.random() * 0.3)
		monster.scriptTable.onTick = function (monster)
			monster.data.oldTick(monster)
			if monster.data.spawnTimer == nil then
				monster.data.spawnTimer = 3.0
			end
			monster.data.spawnTimer = monster.data.spawnTimer - dt
			
			if monster.data.spawnTimer < 0.0 then
				monster.data.spawnTimer = 3.0
				
				if getMonsterCount() < Survival.maxMonster then
					local newMonster = addMonster(monster.monsterTemplate.name)
					newMonster.position = monster.position
					newMonster:setScale(math.max(0.3, monster.scale * 0.60))
					newMonster.colorMultiplier:addBuff(Vec4.new(0.5, 0.3, 0.2, 1.0))
					
					newMonster.data.playerSeeRange = monster.data.playerSeeRange
					newMonster.data.maxMoveSpeed = monster.data.maxMoveSpeed * 0.8
					newMonster.data.maxRotateSpeed = monster.data.maxRotateSpeed

					newMonster.data.hitWaitTime = monster.data.hitWaitTime
					newMonster.data.hitInterval = monster.data.hitInterval
					newMonster.data.minDamage = math.ceil(monster.data.minDamage * 0.4)
					newMonster.data.maxDamage = math.ceil(monster.data.maxDamage * 0.4)

					newMonster.experienceMultiplier = 0.0
					newMonster.scoreMultiplier = 0.0

					newMonster.hitPoint = math.floor(monster.hitPoint * 0.2)
					newMonster.data.randomMove = true
					newMonster.moveAngle = -monster.moveAngle
					
					monster.scriptTable = shallowcopy(monster.scriptTable)
					monster.scriptTable.onKilled = function (monster)
						Survival.ignoreMonsterCount = Survival.ignoreMonsterCount - 1
					end
					Survival.ignoreMonsterCount = Survival.ignoreMonsterCount + 1
				end
			end
		end
	
	end
end


function Survival.init()
	local theSeed = os.time()
	math.randomseed( theSeed )
	Survival.ignoreMonsterCount = 0
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
	Survival.firstKill = true
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
			HomingOrb.spawn(player.position)
	end
	NukeOnDeath.onTick()
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
			monster.experienceMultiplier = 0.0
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
	
    if missionTime - Survival.lastSpawnTime > (0.7 - clamp(min * 0.15) * 0.5) and getMonsterCount() - Survival.ignoreMonsterCount < curMaxMonster and getMonsterCount() < Survival.maxMonster and player.isDead == false then
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


function Survival.onMonsterDied(monster)
	if Survival.firstKill then
		Survival.firstKill = false
		spawnRandomGun(monster.position)
	end
end
