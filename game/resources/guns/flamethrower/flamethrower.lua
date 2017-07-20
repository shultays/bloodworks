FlamethrowerObject = {}

function Flamethrower.init(gun)
	ShootTimer.initGun(gun, 0.05)
	gun.data.gameObject = addGameObject("FlamethrowerObject")
	gun.data.particle = gun.data.gameObject:addParticle("FlameParticle", {})
end

function Flamethrower.onTick(gun)
	if gun.isTriggered and gun:hasAmmo() then
		if ShootTimer.checkGun(gun) then
			gun:consumeAmmo()
			local bullet = gun:addBullet()
			local playerSpeed = player.moveVelocity
			local bulletSpeed = Vec2.new(0.0, 0.0)
			bulletSpeed:setAngle(bullet.moveAngle)
			bulletSpeed = bulletSpeed * bullet.moveSpeed
			local totalSpeed = playerSpeed + bulletSpeed
			bullet.moveSpeed = totalSpeed:length()
			bullet.moveAngle = totalSpeed:getAngle()
			bullet.onDamageArgs.noSlowdown = true
			bullet.penetrateCount = 100
		end
		for i = 1,6 do
			gun.data.particle:addParticle(player.gunPos, {moveSpeed = (player.aimDir * 300.0) + player.moveVelocity})
		end
	end
end



function Flamethrower.onBulletHit(gun, bullet, monster)
	if monster.data.flamethrowerObject == nil then
		monster.data.flamethrowerObject = addGameObject("FlamethrowerMonsterObject")
		monster.data.flamethrowerObject.data.monster = monster
		monster.data.flamethrowerObject.data.time = 0.3
	end
	if monster.data.flamethrowerParticle == nil then
		monster.data.flamethrowerParticle = monster:addParticleSpawner("FlameParticle", {})
	end
	monster.data.flamethrowerObject.data.count = 5
end

FlamethrowerMonsterObject = {}

function FlamethrowerMonsterObject.init(gameobject)
end

function FlamethrowerMonsterObject.onTick(gameobject)
	local data = gameobject.data
	data.time = data.time - dt
	if data.time < 0.0 then
		data.time = data.time + 0.3
		data.monster:doDamageWithArgs(math.floor(math.random() * 3 + 2), Vec2.new(0.0, 0.0), {noSlowdown = true})
		data.count = data.count - 1
		if data.count <= 0 then
			gameobject.toBeRemoved = true
		end
	end

	local t = 0.0
	
	while t < data.monster.bulletRadius do
		local pos = Vec2.new(0.0, 0.0)
		local speed = Vec2.new(0.0, 0.0)
		speed:setAngle(math.random() * math.pi * 2.0)
		speed = speed * (math.random() * 3.0 + 3.0)
		pos:setAngle(math.random() * math.pi * 2.0)
		local r = math.random()
		r = r * r
		pos = pos * (data.monster.bulletRadius * r) 
		
		data.monster.data.flamethrowerParticle:addParticle(data.monster.position + pos, {moveSpeed = data.monster.moveVelocity + speed})
		t = t + 15
	end
	if data.monster.isDead then
		gameobject.toBeRemoved = true
	end
	
	if gameobject.toBeRemoved then
		data.monster.data.flamethrowerObject = nil
	end
end
