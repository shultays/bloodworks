
function clamp(t)
    if t < 0.0 then
        return 0.0
    elseif t > 1.0 then
        return 1.0
    end
    return t
end

function addRandomMonster()
    local m = "Alien"
    local min = time / 60.0 +  math.random() * 1.5 + Survival.extraMin
    if math.random() < 0.1 + clamp(min * 0.5) * 0.2 then
        m = "Spider"
    end
    local monster = addMonster(m)
    monster.data.randomMove = (math.random() > (0.05 + clamp(min * 0.2) * 0.5))
    monster.data.playerSeeRange = monster.data.playerSeeRange * (1.0 +  clamp(min * 0.1) * 2.0)
    monster.data.maxMoveSpeed =  monster.data.maxMoveSpeed * (1.0 + clamp(min * 0.05) * 1.25)
    monster.data.maxRotateSpeed =  monster.data.maxRotateSpeed * (1.0 + clamp(min * 0.05) * 1.0)
	
	
	monster.data.hitWaitTime = monster.data.hitWaitTime  * (1.0 - clamp(min * 0.1) * 0.8)
	monster.data.hitInterval = monster.data.hitInterval  * (1.0 - clamp(min * 0.1) * 0.4)
	monster.data.minDamage = math.floor(monster.data.minDamage * (1.0 + min * 0.2))
	monster.data.maxDamage = math.floor(monster.data.maxDamage * (1.0 + min * 0.2))
	
	monster.hitPoint = math.floor(monster.hitPoint * (1.0 + clamp(min * 0.05) * 1.0))
    return monster
end

function Survival.init()

    Survival.extraMin = 0.0
    Survival.lastSpawnTime = 0.0
	
	
	if DEBUG then
		Survival.maxMonster = 10
	else
		Survival.maxMonster = 350
	end
	
	for i = 1, Survival.maxMonster - 10 do
		local pos = getRandomMonsterSpawnPos()
        local monster = addRandomMonster()
		monster.position = pos
		monster.moveAngle =  math.random() * math.pi * 2.0
	end
    
end

function Survival.onTick()
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
			getMonsterAtIndex(i):doDamage(10000, Vec2.new(1.0, 0.0))
		end
	end
	
	if isKeyReleased(keys.insert) then
		local t = Survival.maxMonster - getMonsterCount()
		for i = 1, t - 10 do
			local pos = getRandomMonsterSpawnPos()
			local monster = addRandomMonster()
			monster.position = pos
			monster.moveAngle =  math.random() * math.pi * 2.0
		end
	end
	
    local min = time / 60.0
    if time - Survival.lastSpawnTime > (1.0 - clamp(min * 10.0) * 0.8) and getMonsterCount() < Survival.maxMonster then
        Survival.lastSpawnTime = time
		local pos = getRandomMonsterSpawnPosOutsideScreen()
        local monster = addRandomMonster()
		monster.position = pos
		monster.moveAngle =  math.random() * math.pi * 2.0
    
    end
end


function Survival.clear()


end
