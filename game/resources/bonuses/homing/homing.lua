
function HomingOrb.spawn(pos)

	if HomingOrb.homingOrbId == nil then
		HomingOrb.homingOrbId = getUniqueId()
	end
	local monster = getClosestMonsterWithIgnoreId(pos, HomingOrb.homingOrbId)
	
	if monster ~= nil then
		monster:addIgnoreId(HomingOrb.homingOrbId)
		local bullet = addCustomBullet()
		bullet.damage = math.floor(math.random() * 30.0 + 30)
		bullet.position = Vec2.new(pos.x, pos.y)
		bullet.moveSpeed = 300.0
		bullet.moveAngle = 0
		bullet.script = HomingOrbBullet
		bullet.onHitCallback = "onHit"
		bullet:addRenderableTextureWithSize("resources/bonuses/homing/bullet.png", Vec2.new(14.0, 14.0))
		
		bullet.data.lastHitIndex = -1
		bullet.radius = 9.0
		
		bullet.moveAngle = (monster.position - pos):getAngle()
		bullet.penetrateCount = math.floor(6 * player.data.bonusDurationMultiplier)
		bullet.damage = math.floor(math.random() * 40 + 30)
		bullet.onTickCallback = "onTick"
		
		bullet.data.monster = monster
		local particle = bullet:addTrailParticle("PlasmaTrailParticle", Vec2.new(0.0, 0.0), 20.0, {})
		particle.args.color = Vec3.new(0.8, 0.7, 0.1)
		particle.args.initialScale = 10.0
		particle.args.initialAlpha = 0.4
		particle.args.scaleSpeed = -20
		particle:setTexture("resources/bonuses/homing/particle.png")
		
		playSound({path = "resources/sounds/plasma.ogg"})
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
	playSound({path = "resources/sounds/plasma_hit.ogg", position = bullet.position})
	bullet.data.monster = getClosestMonsterWithIgnoreId(bullet.position, bullet.index)
	bullet.damage = math.floor(math.random() * 40 + 30)
end

