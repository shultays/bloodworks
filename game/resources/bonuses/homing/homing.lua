
function HomingOrb.spawn(pos)
	local monster = getClosestMonster(pos)
	
	if monster ~= nil then
		
		local bullet = addCustomBullet()
		bullet.damage = math.floor(math.random() * 30.0 + 30)
		bullet.position = Vec2.new(pos.x, pos.y)
		bullet.moveSpeed = 200.0
		bullet.moveAngle = 0
		bullet.script = HomingOrbBullet
		bullet.shouldHitMonsterTest = "shouldHit"
		bullet.onHitCallback = "onHit"
		bullet:addRenderableTextureWithSize("resources/bonuses/homing/bullet.png", Vec2.new(14.0, 14.0))
		
		bullet.data.lastHitIndex = -1
		bullet.radius = 9.0
		
		bullet.moveAngle = (monster.position - pos):getAngle()
		bullet.diesOnHit = false
		bullet.data.lifeLeft = 4
		bullet.damage = math.floor(math.random() * 40 + 30)
	end
end

HomingOrbBullet = {}

function HomingOrbBullet.onHit(bullet, monster)
	bullet.data.lifeLeft = bullet.data.lifeLeft - 1
	if bullet.data.lifeLeft == 0 then
		bullet.diesOnHit = true
		return
	end
	
	bullet.data.lastHitIndex = monster.index
	local ignoreId = "ignoreHoming" .. bullet.index
	
	monster.data[ignoreId] = true
	
	local newMonster = getClosestMonsterWithIgnoreData(bullet.position, ignoreId)

	if newMonster == nil then
		bullet.diesOnHit = true
		return
	end
	
	bullet.damage = math.floor(math.random() * 40 + 30)
	bullet.moveAngle = (newMonster.position - bullet.position):getAngle()
end

function HomingOrbBullet.shouldHit(bullet, monster)
	if monster.index == bullet.data.lastHitIndex
	then
		return false
	end
	return true
end