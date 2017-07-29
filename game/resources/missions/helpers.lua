
function addRandomMonster(forceType, cannotBecomeBoss, cannotShootBullets, levelReduce)
    local min = missionTime / 60.0 +  math.random() * 1.5 + missionData.extraMin
	
	if levelReduce ~= nil then
		min = min - levelReduce
	end
	
	if forceType ~= nil then
		m = forceType
	else
		local randChance = math.random() * missionData.totalChanceSpawn
		for key,value in pairs(missionData.spawnRates) do
			randChance = randChance - value
			if randChance <= 0.000001 then
				m = key
				break
			end
		end

		if m == "" then
			print("error not picked mon type")
			m = "Alien"
		end
	end
	
    local monster = addMonster(m)
	
	monster.hitPoint = math.floor(monster.hitPoint * (1.0 + clamp(min * 0.05) * 1.0))
	monster.experienceMultiplier = math.random() * 0.4 + 0.8
	monster.experienceMultiplier = 0.9 + math.random() * 0.2
	monster.scoreMultiplier = 0.9 + math.random() * 0.2
	
	if cannotBecomeBoss ~= true and monster.scriptTable.makeBoss ~= nil and missionData.lastBossSpawn + 20.0 - clamp(min/7) * 10 < missionTime then
		missionData.lastBossSpawn = missionTime
		monster.data.isBoss = true
		monster.scriptTable.makeBoss(monster, min)
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
	
	if monster.scriptTable.buffStats ~= nil then
		monster.scriptTable.buffStats(monster, min)
	end
	
    return monster
end

function makeBossDefault(monster)
    local min = missionTime / 60.0 +  math.random() * 1.5 + missionData.extraMin
	monster.hitPoint = math.floor(monster.hitPoint * 1.5)
	
	monster.data.minDamage = math.floor(monster.data.minDamage * 1.1)
	monster.data.maxDamage = math.floor(monster.data.maxDamage * 1.1)
	
	monster.data.maxMoveSpeed = monster.data.maxMoveSpeed * 1.05
	monster.data.randomMove = false
	monster.data.playerSeeRange = monster.data.playerSeeRange * 1.5
	
	monster.experienceMultiplier = 5.0 + math.random() * 2.0
	monster.scoreMultiplier = 5.0 + math.random() * 2.0
	monster:modifyDrawLevel(3)
	local t = math.random(11)
	--t = 6
	
	if t == 1 then
		monster.hitPoint = monster.hitPoint * 7
		monster.colorMultiplier:addBuff(Vec4.new(0.9, 0.8, 0.3, 1.0))
		monster:setScale(1.0 + math.random() * 0.3)
		monster.knockbackResistance:addBuff(0.07)
	elseif t == 2 then
		monster.colorMultiplier:addBuff(Vec4.new(0.5, 0.5, 0.5, 0.5))
		monster:setScale(monster.scale * 0.85)
	elseif t == 3 then
		monster.colorMultiplier:addBuff(Vec4.new(1.0, 0.3, 0.3, 1.0))
		monster.data.minDamage = math.floor(monster.data.minDamage * 2.0)
		monster.data.maxDamage = math.floor(monster.data.maxDamage * 2.0)
	elseif t == 4 then
		monster.colorMultiplier:addBuff(Vec4.new(0.2, 0.7, 1.0, 1.0))
		monster.data.maxMoveSpeed = monster.data.maxMoveSpeed * 1.85
		monster.data.minDamage = math.floor(monster.data.minDamage * 0.8)
		monster.data.maxDamage = math.floor(monster.data.maxDamage * 0.8)
		monster.knockbackResistance:addBuff(0.4)
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
		monster:setScale(0.8 + math.random() * 0.3)
		monster.experienceMultiplier = monster.experienceMultiplier * 0.3
		monster.scoreMultiplier = monster.scoreMultiplier * 0.3
		monster.data.onKillFuncSplit = function (monster)
			if monster.data.remainingLife > 0 then
				monster.data.remainingLife = monster.data.remainingLife - 1
				for i = 1,2 do
					local newMonster = addMonster(monster.monsterTemplate.name)
					newMonster.data.remainingLife = monster.data.remainingLife
					newMonster.position = monster.position
					newMonster:setScale(math.max(0.3, monster.scale * 0.80))
					newMonster.colorMultiplier:addBuff(Vec4.new(0.7, 0.2, 0.7, 1.0))
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
					
					addCustomOnKill(newMonster, monster.data.onKillFuncSplit)
				end
			end
		end
		addCustomOnKill(monster, monster.data.onKillFuncSplit)
	elseif t == 7 then
		monster.colorMultiplier:addBuff(Vec4.new(0.2, 0.2, 0.2, 1.0))
		monster:setScale(0.8 + math.random() * 0.3)
		monster.experienceMultiplier = monster.experienceMultiplier * 0.5
		monster.scoreMultiplier = monster.scoreMultiplier * 0.5
		addCustomOnKill(monster, function (monster)
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
		end)
	elseif t == 8 then
		monster.colorMultiplier:addBuff(Vec4.new(2.0, 2.0, 2.0, 1.0))
		monster.data.maxMoveSpeed = monster.data.maxMoveSpeed * 3.5
		monster.data.originalSpeed = monster.data.maxMoveSpeed
		monster.data.maxRotateSpeed =  monster.data.maxRotateSpeed * 3.0
		monster.data.originalRotateSpeed =  monster.data.maxRotateSpeed
			
		monster.data.hitWaitTime = monster.data.hitWaitTime * 0.1
		monster.data.hitInterval = monster.data.hitInterval * 0.2
		monster.data.targetShift = Vec2.new(0.0, 0.0)
		monster.knockbackResistance:addBuff(0.0)
		addCustomOnTick(monster, function (monster)
			local diffToPlayer = player.position - monster.position 
			local distanceToPlayer = diffToPlayer:length()
			local angleToPlayer = diffToPlayer:getAngle()
			local a = fixAngle(angleToPlayer - player.aimAngle)
			local angle = 0.2
			if distanceToPlayer < 100 then
				angle = angle + 0.2 * (1.0 - distanceToPlayer / 100.0)
			end
			
			if math.abs(a) > math.pi - angle then
				monster.data.maxMoveSpeed = 0.0
				monster.animationSpeed = 0.0
				monster.moveSpeed = 0.0
				monster.data.maxRotateSpeed = 0.0
				monster.data.canHit = false
			else
				monster.data.maxMoveSpeed = monster.data.originalSpeed
				monster.animationSpeed = 1.0
				monster.data.maxRotateSpeed = monster.data.originalSpeed
				monster.data.canHit = true
			end
		end)
	elseif t == 9 then
		monster.colorMultiplier:addBuff(Vec4.new(0.7, 1.7, 0.7, 1.0))
		addCustomOnHit(monster, function(monster, damage, args)
			local buffId = monster.colorMultiplier:addBuff(Vec4.new(1.0, 1.0, 1.0, 0.2))
			monster.colorMultiplier:setBuffDuration(buffId, 1.0)
			monster.data.lastHitTime = time
		end)
		addCustomShouldHit(monster, function(monster, gun, bullet)
			return monster.data.lastHitTime == nil or time - monster.data.lastHitTime > 1.0
		end)
	elseif t == 10 then
		monster.data.blinkParticle = monster:addParticleSpawner("CriticalParticle", {});
		monster.colorMultiplier:addBuff(Vec4.new(0.7, 1.7, 1.7, 1.0))
		monster.hitPoint = math.floor(monster.hitPoint * 1.5)

		addCustomOnHit(monster, function(monster, damage, args)
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
				
				playSound({path = "~/resources/sounds/shimmer_1.ogg"})
			end
		end)
	elseif t == 11 then
		monster.colorMultiplier:addBuff(Vec4.new(0.5, 0.3, 0.2, 1.0))
		monster.moveSpeed = monster.moveSpeed * 0.5
		monster.data.maxHitpoint = monster.hitPoint
		monster:setScale(0.8 + math.random() * 0.3)
		
		addCustomOnTick(monster, function (monster)
			if monster.data.spawnTimer == nil then
				monster.data.spawnTimer = 3.0
			end
			monster.data.spawnTimer = monster.data.spawnTimer - dt
			
			if monster.data.spawnTimer < 0.0 then
				monster.data.spawnTimer = monster.data.spawnTimer + 3.0
				
				if missionData.ignoreMonsterCount < 50 then
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

					newMonster.hitPoint = math.floor(monster.data.maxHitpoint * 0.2)
					newMonster.data.randomMove = true
					newMonster.moveAngle = -monster.moveAngle
					
					ignoreMonsterForCount(newMonster)
				end
			end
		end)
	end
end

function removeBuffIcon(name, dontUpdate)
	local buffData = missionData.buffs[name] 
	if buffData ~= nil then
		missionData.buffObject:removeRenderable(buffData.renderable)
		missionData.buffs[name] = nil
		missionData.buffCount = missionData.buffCount - 1
		
		if dontUpdate ~= true then
			updateBuffPositions()
		end
	end
end

function addBuffIcon(name, path)
	if missionData.buffObject == nil then
		missionData.buffObject = addGameObject("BuffIcons")
		missionData.buffObject:setAlignment(RenderableAlignment.top)
		missionData.buffObject:setLevel(RenderableLevel.GUI)
		BuffIcons.repositionGUI(missionData.buffObject)
		missionData.buffs = {}
		missionData.buffCount = 0
	end
	
	removeBuffIcon(name, true)
	
	local buffObject = missionData.buffObject
	local renderable = buffObject:addTexture(path, "~/resources/default")
	renderable.textureSize = Vec2:new(15.0, 15.0)
	renderable.position = Vec2:new(0, 0)
	renderable.alignment = RenderableAlignment.top
	renderable:update()
	
	local buffData = {}
	buffData.renderable = renderable.id
	buffData.name = name
	buffData.id = getUniqueId()
	missionData.buffs[name] = buffData
	missionData.buffCount = missionData.buffCount + 1
	
	updateBuffPositions()
end

function updateBuffPositions()
	local p = 0
	local shift = (missionData.buffCount - 1) * 34.0 / 2
	
	for key,value in pairs(missionData.buffs) do
		local renderable = missionData.buffObject:getRenderable(value.renderable)
		renderable.position = Vec2:new(p * 34 - shift, 0)
		renderable:update()
		p = p + 1
	end
end

BuffIcons = {}
function BuffIcons.init()
end
function BuffIcons.repositionGUI(gameObject)
	gameObject:setPosition(Vec2.new(0, -90 / getCameraZoom()))
	gameObject:setScale(Vec2.new(1.0 / getCameraZoom(), 1.0 / getCameraZoom()))
end


function ignoreMonsterForCount(monster)
	if monster.data.ignoreForCount ~= true then
		monster.data.ignoreForCount = true
		missionData.ignoreMonsterCount = missionData.ignoreMonsterCount + 1
	end
end

function canSpawnMonster()
	return (missionData.maxMonster == nil or getMonsterCount() < missionData.maxMonster + missionData.ignoreMonsterCount)
end

function canSpawnIgnoredMonster()
	return (missionData.maxMonster == nil or getMonsterCount() < missionData.maxMonster) and missionData.ignoreMonsterCount < 50
end