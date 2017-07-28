
HomingOrb.homingOrbId = getGlobalUniqueId()

function HomingOrb.spawn(bonus, pos)

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
		bullet:addRenderableTextureWithSize(HomingOrb.basePath .. "bullet.png", Vec2.new(14.0, 14.0))
		
		bullet.data.lastHitIndex = -1
		bullet.radius = 12.0
		
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
		particle:setTexture(HomingOrb.basePath .. "particle.png")
		
		playSound({path = "~/resources/sounds/plasma.ogg"})
	end
end

HomingOrbBullet = {}

function HomingOrbBullet.onTick(bullet)

	if bullet.data.monster ~= nil then
		local toMonster = (bullet.data.monster.position - bullet.position)
		local m = 1.0 - clamp(toMonster:length() / 60)
		bullet.moveAngle = approachAngle(bullet.moveAngle, toMonster:getAngle(), (10 + 90 * m) * dt)
		if bullet.data.monster.isDead or bullet.data.monster:hasIgnoreId(HomingOrb.homingOrbId) then
			bullet.data.monster = getClosestMonsterWithIgnoreId(bullet.position, HomingOrb.homingOrbId)
		end
	end
end

function HomingOrbBullet.onHit(bullet, monster)
	playSound({path = "~/resources/sounds/plasma_hit.ogg", position = bullet.position})
	monster:addIgnoreId(HomingOrb.homingOrbId)
	bullet.data.monster = getClosestMonsterWithIgnoreId(bullet.position, HomingOrb.homingOrbId)
	bullet.damage = math.floor(math.random() * 40 + 30)
end

