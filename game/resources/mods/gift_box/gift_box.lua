
function GiftBox.init(monster, min)
	data = monster.data
	monster:setScale(math.random() * 0.2 + 0.8)

	monster.moveSpeed = 0
	monster.moveAngle =  math.random() * math.pi * 2.0
	monster:playAnimation("stand", math.random())
	
	monster:modifyDrawLevel(3)
	
	data.cannotBecomeBoss = true
	data.cannotShootBullets = true
	
	data.angle = monster.moveAngle
	data.rotate = 0.2 + math.random() * 0.1
	data.shift = math.random() * math.pi

	monster.data.particle = monster:addParticleSpawner("CriticalParticle", {});
	
	monster.knockbackResistance:addBuff(0.0)
	
	monster.dropChance = 0.0
	missionData.giftBoxTime = time
end

function GiftBox.onMissionLoad(missionData)
	local m = addRandomMonster("GiftBox")
	m.position = player.position + Vec2.new(0, 200)
	m.data.spawnType = "gun"
	
	missionData.spawnWeaponOnFirstKill = false
end


function GiftBox.spawnChanceInMission(missionData, min)
	if time - missionData.giftBoxTime > 40.0 then
		return 0.00
	end
	return 0.05
end

function GiftBox.onTick(monster)
	monster.moveAngle = monster.data.angle + math.sin(time * 6.0 + monster.data.shift) * monster.data.rotate

	local s = Vec2:new()
	s:setAngle(math.random() * math.pi * 2.0)
	s = s * 20

	monster:spawnParticleShifted(s, monster.data.particle, {initialScale = 15.0, moveSpeed = 150.0})
end

function GiftBox.onKilled(monster)
	if monster.data.spawnType == "gun" then
		spawnRandomGun(monster.position)
	else
		spawnRandomBonus(monster.position)
	end
end