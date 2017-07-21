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
		gun.playFiringSound = true
	else
		gun.playFiringSound = false
	end
end

function Flamethrower.onBulletHit(gun, bullet, monster)
	if monster.data.flamethrowerObject == nil then
		monster.data.flamethrowerObject = addGameObject("BurnMonsterObject")
		monster.data.flamethrowerObject.data.monster = monster
		monster.data.flamethrowerObject.data.time = 0.3
		monster.data.flamethrowerObject.data.damageMin = 2
		monster.data.flamethrowerObject.data.damageVar = 3
	end
	if monster.data.burnParticle == nil then
		monster.data.burnParticle = monster:addParticleSpawner("FlameParticle", {})
	end
	monster.data.flamethrowerObject.data.count = 4
end

