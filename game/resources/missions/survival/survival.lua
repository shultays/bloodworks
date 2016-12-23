
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
    local min = time / 60.0
    
    if math.random() < 0.1 then
        m = "Spider"
    end
    local monster = addMonster(m)
    monster.data.randomMove = (math.random() > (0.05 + clamp(min * 0.2) * 55))
    monster.data.playerSeeRange = monster.data.playerSeeRange * (1.0 +  clamp(min * 0.1) * 3.0)
    monster.data.maxMoveSpeed =  monster.data.maxMoveSpeed * (1.0 + clamp(min * 0.05) * 2.0)
    monster.data.maxRotateSpeed =  monster.data.maxRotateSpeed * (1.0 + clamp(min * 0.05) * 1.0)
    return monster
end

function Survival.init()

	for i = 1, 350 do
		local pos = getRandomMonsterSpawnPos()
        local monster = addRandomMonster()
		monster.position = pos
		monster.moveAngle =  math.random() * math.pi * 2.0
	end
    
    Survival.lastSpawnTime = 0.0
end

function Survival.onTick()
    local min = time / 60.0
    if time - Survival.lastSpawnTime > (1.0 - clamp(min * 10.0) * 0.8) and getMonsterCount() < 355 then
        Survival.lastSpawnTime = time
		local pos = getRandomMonsterSpawnPosOutsideScreen()
        local monster = addRandomMonster()
		monster.position = pos
		monster.moveAngle =  math.random() * math.pi * 2.0
    
    end
end


function Survival.clear()


end
