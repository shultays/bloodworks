
function Egg.init(monster, min)
	data = monster.data
	monster:setScale(math.random() * 0.2 + 0.8)

	monster.moveSpeed = 0
	monster.moveAngle =  math.random() * math.pi * 2.0
	
	monster:playAnimation("stand", math.random())
	
	monster:modifyDrawLevel(2)
	monster.colorMultiplier:addBuff(Vec4.new(0.6, 0.6, 0.6, 1.0))
	
    data.maxMoveSpeed = 0.0
    data.maxRotateSpeed = 0.0
    data.playerSeeRange = 0.0
	data.minDamage = 0
	data.maxDamage = 0
	
	data.hitWaitTime = 0.0
	data.hitInterval = 0.0
	
	data.spawnInterval = 3.0 + math.random() * 0.5
	data.spawnTime = data.spawnInterval * (1.0 + math.random())
	
	data.cannotBecomeBoss = true
	data.cannotShootBullets = true
	data.monsterType = "Alien"
	if math.random() < 0.07 then
		data.monsterType = "Spider"
	end
	monster.knockbackResistance:addBuff(0.1)
end

function Egg.spawnChanceInMission(missionData, min)
	return 0.05 + clamp(min * 0.3) * 0.04
end

function Egg.buffStats(monster, min)
	monster.data.spawnInterval = monster.data.spawnInterval - clamp(min/10)
end

function Egg.onHit(monster, damage, args)
end

function Egg.onTick(monster)
    data = monster.data
	
	data.spawnTime = data.spawnTime - dt
	
	if data.spawnTime < 0.0 then
		data.spawnTime = data.spawnTime + data.spawnInterval
		
		if (missionData.maxMonster == nil or getMonsterCount() < missionData.maxMonster) and missionData.ignoreMonsterCount < 50 then
			local m = addRandomMonster(data.monsterType, true, true, 1.0)
			m:setScale(m.scale * 0.8)
			m.position = monster.position
			Survival.ignoreMonsterForCount(m)
		end
	end
end
