
function RocketLauncher.init(gun)
	gun.data.timeToNextShoot = 0.0
	gun.spreadAngle = 0.0
	gun.crosshairDistance = 350.0
end


function RocketLauncher.onTick(gun)
	if gun.leftMouseDown then
		if gun.data.timeToNextShoot < time then
			gun.data.timeToNextShoot = time + 0.8
			
			local bullet = gun:addBullet()
			bullet.damage = math.floor(110 + math.random() * 40)
			bullet:addTrailParticle("RocketSmoke", Vec2.new(0.0, -8.0), {})
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
	bullet.data.lifeTime = bullet.data.lifeTime - dt
	
	if bullet.data.lifeTime < 0.0 then
		RocketLauncher.onBulletHit(gun, bullet, nil)
		bullet:removeSelf()
	end
	bullet.data.moveSpeed = bullet.data.moveSpeed + dt * 250
	if bullet.data.moveSpeed > 250.0 then
		bullet.data.moveSpeed = 250.0
	end
	
	bullet.data.lateralSpeed = bullet.data.lateralSpeed - dt * 0.5
	
	if bullet.data.lateralSpeed < 0.0 then
		bullet.data.lateralSpeed = 0.0
	end
	
	local defaultSpeed = Vec2.new(0.0, 0.0)
	defaultSpeed:setAngle(bullet.data.moveAngle)
	defaultSpeed = defaultSpeed * bullet.data.moveSpeed
	
	local finalSpeed = defaultSpeed + bullet.data.lateralSpeedDir * bullet.data.lateralSpeed
	
	bullet.moveSpeed = finalSpeed:length()
	bullet.moveAngle = finalSpeed:getAngle()
end
