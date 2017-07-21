ShootTimer = {}

function ShootTimer.initGun(gun, interval)
	gun.data.shootInterval = interval
	gun.data.timeToNextShoot = 0.0
end

function ShootTimer.IsGunReady(gun)
	return gun.data.timeToNextShoot < time and dt > 0.0
end

function ShootTimer.GunShot(gun)
	player:playShootAnimation()
	gun.data.timeToNextShoot = time + gun.data.shootInterval * player.shootSpeedMultiplier:getBuffedValue()
end


function ShootTimer.checkGun(gun)
	if gun.data.timeToNextShoot < time and dt > 0.0 then
		player:playShootAnimation()
		gun.data.timeToNextShoot = time + gun.data.shootInterval * player.shootSpeedMultiplier:getBuffedValue()
		return true
	end
	return false
end


SpreadHelper = {}

function SpreadHelper.initGun(gun)
	gun.data.timeToNextShoot = time
	gun.spreadAngle = 0.0
	gun.data.minSpread = 0.0
	gun.data.maxSpread = 0.15
	gun.data.spreadDecreaseStartTime = 0.1
	gun.data.spreadDecreaseSpeed = 0.15
	gun.data.spreadIncreasePerShoot = 0.025
end

function SpreadHelper.onTick(gun)
	local data = gun.data
	if data.timeToNextShoot < time - data.spreadDecreaseStartTime then
		gun.spreadAngle = gun.spreadAngle - dt * data.spreadDecreaseSpeed
	end
	
	if gun.spreadAngle < data.minSpread then
		gun.spreadAngle = data.minSpread
	end
end



function SpreadHelper.onShoot(gun)
	local data = gun.data
	gun.spreadAngle = gun.spreadAngle + data.spreadIncreasePerShoot
	if gun.spreadAngle > gun.data.maxSpread then
		gun.spreadAngle = gun.data.maxSpread
	end
end

BurnMonsterObject = {}

function BurnMonsterObject.init(gameobject)
end

function BurnMonsterObject.onTick(gameobject)
	local data = gameobject.data

	data.time = data.time - dt
	if data.time < 0.0 then
		data.time = data.time + 0.3
		data.monster:doDamageWithArgs(math.floor(math.random() * data.damageVar + data.damageMin), Vec2.new(0.0, 0.0), {noSlowdown = true})
		data.count = data.count - 1
		if data.count <= 0 then
			gameobject.toBeRemoved = true
			data.monster.data.flamethrowerObject = nil
		end
	end

	
	if data.monster.isDead then
		gameobject.toBeRemoved = true
	else
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
			
			data.monster.data.burnParticle:addParticle(data.monster.position + pos, {moveSpeed = data.monster.moveVelocity + speed})
			t = t + 15
		end
	end
	
end