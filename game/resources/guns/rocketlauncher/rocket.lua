
function RocketLauncher.init(gun)
	gun.spreadAngle = 0.0
	gun.crosshairDistance = 350.0
	ShootTimer.InitGun(gun, 0.5)
end


function RocketLauncher.onTick(gun)
	if gun.leftMouseDown then
		if ShootTimer.CheckGun(gun) then
			local bullet = gun:addBullet()
			bullet.damage = math.floor(110 + math.random() * 40)
			bullet:addTrailParticle("RocketSmokeParticle", Vec2.new(0.0, -8.0), {})
			bullet.moveSpeed = 50
			bullet.radius = 6
			bullet.data.moveAngle = bullet.moveAngle
			bullet.data.moveSpeed = bullet.moveSpeed
			bullet.data.lateralSpeedDir = player.moveSpeedDir - (player.moveDir * player.moveSpeedDir:dot(player.aimDir))
			bullet.data.lateralSpeed = bullet.data.lateralSpeedDir:safeNormalize() * 0.3
			bullet.data.lifeTime = 2.5
			bullet.meshRotation = bullet.moveAngle
		end
	end
end

function RocketLauncher.onBulletHit(gun, bullet, monster)
	addExplosion(bullet.position, 40.0, 80.0, 70, 130)
end

function RocketLauncher.onBulletTick(gun, bullet)
    local data = bullet.data
	data.lifeTime = data.lifeTime - dt
	
	if data.lifeTime < 0.0 then
		RocketLauncher.onBulletHit(gun, bullet, nil)
		bullet:removeSelf()
	end
	data.moveSpeed = data.moveSpeed + dt * 250
	if data.moveSpeed > 250.0 then
		data.moveSpeed = 250.0
	end
	
	data.lateralSpeed = data.lateralSpeed - dt * 0.5
	
	if data.lateralSpeed < 0.0 then
		data.lateralSpeed = 0.0
	end
	
	local defaultSpeed = Vec2.new(0.0, 0.0)
	defaultSpeed:setAngle(data.moveAngle)
	defaultSpeed = defaultSpeed * data.moveSpeed
	
	local finalSpeed = defaultSpeed + data.lateralSpeedDir * data.lateralSpeed
	
	bullet.moveSpeed = finalSpeed:length()
	bullet.moveAngle = finalSpeed:getAngle()
end
