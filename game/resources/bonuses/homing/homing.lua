
function HomingOrb.spawn(pos)
	local monster = getClosestMonster(pos)
	
	if monster ~= nil then
		
		local bullet = addCustomBullet()
		bullet.damage = math.floor(math.random() * 30.0 + 30)
		bullet.position = Vec2.new(pos.x, pos.y)
		bullet.moveSpeed = 300.0
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
		bullet.onTickCallback = "onTick"
		
		bullet.data.monster = monster
		local particle = bullet:addTrailParticle("PlasmaTrailParticle", Vec2.new(0.0, 0.0), 20.0, {})
		particle.args.color = Vec3.new(0.8, 0.7, 0.1)
		particle.args.initialScale = 10.0
		particle.args.initialAlpha = 0.4
		particle.args.scaleSpeed = -20
		particle:setTexture("resources/bonuses/homing/particle.png")
	end
end

HomingOrbBullet = {}

function HomingOrbBullet.onTick(bullet)

	if bullet.data.monster ~= nil then
	
		
		bullet.moveAngle = approachAngle(bullet.moveAngle, (bullet.data.monster.position - bullet.position):getAngle(), 10 * dt)
		if bullet.data.monster.isDead then
			bullet.onTickCallback = ""
			bullet.data.monster = nil
		end
	end
end

function HomingOrbBullet.onHit(bullet, monster)
	bullet.data.lifeLeft = bullet.data.lifeLeft - 1
	if bullet.data.lifeLeft == 0 then
		bullet.diesOnHit = true
		return
	end
	
	bullet.data.lastHitIndex = monster.index
	
	monster:addIgnoreId(bullet.index)
	
	local newMonster = getClosestMonsterWithIgnoreId(bullet.position, bullet.index)

	if newMonster == nil then
		bullet.diesOnHit = true
		return
	end
	bullet.data.monster = newMonster
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