
function Egg.init(monster, min)
	data = monster.data
	monster:setScale(math.random() * 0.4 + 1.5)

	monster.moveSpeed = 0
	monster.moveAngle =  math.random() * math.pi * 2.0
	
	monster:playAnimation("stand", math.random())
	
	monster:modifyDrawLevel(2)
	
    data.maxMoveSpeed = 0.0
    data.maxRotateSpeed = 0.0
    data.playerSeeRange = 0.0
	data.minDamage = 0
	data.maxDamage = 0
	
	data.hitWaitTime = 0.0
	data.hitInterval = 0.0
	
	data.spawnInterval = 4.0 + math.random() * 1.0
	data.spawnTime = data.spawnInterval * 1.5
	
	data.cannotBecomeBoss = true
	data.cannotShootBullets = true
end

function Egg.onHit(monster, damage, args)
end

function Egg.onTick(monster)
    data = monster.data
	
	data.spawnTime = data.spawnTime - dt
	
	if data.spawnTime < 0.0 then
		data.spawnTime = data.spawnTime + data.spawnInterval
		
		if getMonsterCount() < Survival.maxMonster then
			local m = addRandomMonster("Alien", true, true, 1.0)
			m:setScale(m.scale * 0.8)
			m.position = monster.position
		end
	end
end
